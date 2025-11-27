// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <unistd.h>

#include "filesystem.hpp"
#include "file_info.hpp"
#include "file_info_maps.hpp"
#include "file_info_sets.hpp"
#include "incremental_args.hpp"
#include "incremental_base.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class incremental_work
      : public incremental_base
   {
   public:
      incremental_work( const std::filesystem::path& source_dir, const std::filesystem::path new_backup, const incremental_args args )
         : incremental_base( source_dir, new_backup ),
           m_args( args )
      {
         FILEZ_STDOUT( "Creating directory hierarchy..." );
         constexpr auto opts = std::filesystem::copy_options::recursive | std::filesystem::copy_options::directories_only | std::filesystem::copy_options::skip_symlinks;
         std::filesystem::copy( m_src_path, m_new_path, opts );  // TODO: This is slower than expected, optimise?
      }

      void backup()
      {
         FILEZ_STDOUT( "Copying and hard linking files..." );

         for( const auto& fi : m_src_files ) {
            if( fi->stat().is_file() ) {
               backup( *fi );
            }
         }
         FILEZ_STDOUT( "Empty files: " << m_empty_files );
         FILEZ_STDOUT( "Files linked: " << m_linked_files );
         FILEZ_STDOUT( "Bytes linked: " << m_linked_bytes );
         FILEZ_STDOUT( "Files copied: " << m_copied_files );
         FILEZ_STDOUT( "Bytes copied: " << m_copied_bytes );
         FILEZ_STDOUT( "Old backup files: " << m_old_files.size() );
         // FILEZ_STDOUT( "Old unused files: TODO" );
      }

   private:
      std::size_t m_empty_files = 0;
      std::size_t m_copied_files = 0;
      std::size_t m_copied_bytes = 0;
      std::size_t m_linked_files = 0;
      std::size_t m_linked_bytes = 0;

      const incremental_args m_args;

      void backup( file_info& fi )
      {
         if( fi.path().native().ends_with( ".DS_Store" ) ) {
            return;
         }
         const auto to = transfer( fi.path(), m_src_path, m_new_path );

         if( fi.stat().size() == 0 ) {
            backup_empty( to );
         }
         else if( fi.stat().size() < m_args.c ) {
            backup_copy( fi, to );
         }
         else if( !backup_link( fi, to ) ) {
            backup_copy( fi, to );
         }
      }

      void backup_empty( const std::filesystem::path& to )
      {
         create_empty_file( to );
         ++m_empty_files;
         FILEZ_STDOUT( "Create: " << to );
      }

      [[nodiscard]] bool backup_link( file_info& fi, const std::filesystem::path& to )
      {
         const auto iter = m_old_files.find( fi.stat().size() );

         if( iter == m_old_files.end() ) {
            return false;
         }
         if( m_args.P ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               if( transferred( m_src_path, fi.path(), of->path() ) ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         if( m_args.p ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               FILEZ_ASSERT( !fi.path().filename().empty() );
               FILEZ_ASSERT( !of->path().filename().empty() );

               if( fi.path().filename() == of->path().filename() ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         if( m_args.h ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               if( of->smart_hash() == fi.smart_hash() ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         if( m_args.n ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               FILEZ_ASSERT( !fi.path().filename().empty() );
               FILEZ_ASSERT( !of->path().filename().empty() );

               if( ( fi.path().filename() == of->path().filename() ) && ( of->smart_hash() == fi.smart_hash() ) ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         if( m_args.H ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               FILEZ_ASSERT( !fi.path().filename().empty() );
               FILEZ_ASSERT( !of->path().filename().empty() );

               if( ( fi.path().filename() == of->path().filename() ) && ( of->total_hash() == fi.total_hash() ) ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         if( m_args.N ) {
            for( const std::shared_ptr< file_info >& of : iter->second ) {
               if( of->total_hash() == fi.total_hash() ) {
                  backup_link_impl( *of, to );
                  return true;
               }
            }
         }
         return false;
      }

      void backup_link_impl( file_info& of, const std::filesystem::path& to )
      {
         hard_link_impl( of.path(), to );
         ++m_linked_files;
         m_linked_bytes += of.stat().size();
         FILEZ_STDOUT( "Link: " << of.path() << " -> " << to );
      }

      void backup_copy( file_info& fi, const std::filesystem::path& to )
      {
         copy_file_impl( fi.path(), to );

         if( m_args.x ) {
            add( std::make_shared< file_info >( to ) );
         }
         ++m_copied_files;
         m_copied_bytes += fi.stat().size();
         FILEZ_STDOUT( "Copy: " << fi.path() << " -> " << to );
      }
   };

}  // namespace filez
