// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <unistd.h>
#include <vector>

#include "deduplicate_args.hpp"
#include "deduplicate_base.hpp"
#include "file_info.hpp"
#include "file_info_maps.hpp"
#include "file_info_sets.hpp"
#include "filesystem.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class deduplicate_work
      : public deduplicate_base
   {
   public:
      deduplicate_work( const std::filesystem::path& source_dir, const std::filesystem::path merged_dir, const deduplicate_args args )
         : deduplicate_base( source_dir, merged_dir ),
           m_args( args )
      {
         FILEZ_STDOUT( "Creating directory hierarchy..." );
         constexpr auto opts = std::filesystem::copy_options::recursive | std::filesystem::copy_options::directories_only | std::filesystem::copy_options::skip_symlinks;
         std::filesystem::copy( m_src_path, m_new_path, opts );  // TODO: This is slower than expected, optimise?
      }

      void merge()
      {
         FILEZ_STDOUT( "Hard linking files..." );

         for( const auto& kv : m_src_files ) {
            merge( kv.second );
         }
         FILEZ_STDOUT( "Empty files: " << m_empty_files );
         FILEZ_STDOUT( "Files linked: " << m_linked_files );
         FILEZ_STDOUT( "Bytes linked: " << m_linked_bytes );
         FILEZ_STDOUT( "Files copied: " << m_copied_files );
         FILEZ_STDOUT( "Bytes copied: " << m_copied_bytes );
      }

   private:
      std::size_t m_empty_files = 0;
      std::size_t m_copied_files = 0;
      std::size_t m_copied_bytes = 0;
      std::size_t m_linked_files = 0;
      std::size_t m_linked_bytes = 0;

      const deduplicate_args m_args;

      void merge( const std::vector< std::shared_ptr< file_info > >& fs )
      {
         for( const auto& fi : fs ) {
            if( fi->stat().is_file() ) {
               merge( fs, *fi );
            }
         }
      }

      void merge( const std::vector< std::shared_ptr< file_info > >& fs, file_info& fi )
      {
         if( fi.path().native().ends_with( ".DS_Store" ) ) {
            return;
         }
         const auto to = transfer( fi.path(), m_src_path, m_new_path );

         if( fi.stat().size() == 0 ) {
            merge_empty( to );
         }
         else if( fi.stat().size() < m_args.c ) {
            merge_copy( fi, to );
         }
         else {
            merge_link( fs, fi, to );
         }
      }

      void merge_empty( const std::filesystem::path& to )
      {
         create_empty_file( to );
         ++m_empty_files;
         FILEZ_STDOUT( "Create: " << to );
      }

      void merge_link( const std::vector< std::shared_ptr< file_info > >& fs, file_info& fi, const std::filesystem::path& to )
      {
         if( m_args.h ) {
            const auto& h = fi.smart_hash();

            for( const std::shared_ptr< file_info >& of : fs ) {
               if( of->smart_hash() == h ) {
                  merge_link_impl( *of, to );
                  return;
               }
            }
            FILEZ_ASSERT( false );
         }
         if( m_args.H ) {
            const auto& h = fi.total_hash();

            for( const std::shared_ptr< file_info >& of : fs ) {
               if( of->total_hash() == h ) {
                  merge_link_impl( *of, to );
                  return;
               }
            }
            FILEZ_ASSERT( false );
         }
         FILEZ_ASSERT( false );
      }

      void merge_link_impl( file_info& of, const std::filesystem::path& to )
      {
         hard_link_impl( of.path(), to );
         ++m_linked_files;
         m_linked_bytes += of.stat().size();
         FILEZ_STDOUT( "Link: " << of.path() << " -> " << to );
      }

      void merge_copy( file_info& fi, const std::filesystem::path& to )
      {
         copy_file_impl( fi.path(), to );
         ++m_copied_files;
         m_copied_bytes += fi.stat().size();
         FILEZ_STDOUT( "Copy: " << fi.path() << " -> " << to );
      }
   };

}  // namespace filez
