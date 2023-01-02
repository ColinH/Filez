// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>

#include "create_file.hpp"
#include "file_info.hpp"
#include "file_info_maps.hpp"
#include "file_info_sets.hpp"
#include "incremental_base.hpp"
#include "macros.hpp"

namespace filez
{
   class incremental_work
      : private incremental_base
   {
   public:
      incremental_work( const std::filesystem::path& source_dir, const std::filesystem::path old_backup, const std::filesystem::path new_backup )
         : incremental_base( source_dir, old_backup, new_backup ),
           m_src_files( make_full_file_info_by_path_set( m_src_path ) ),
           m_old_files( make_full_file_info_by_size_map( m_old_path ) )
      {
         constexpr auto opts = std::filesystem::copy_options::recursive | std::filesystem::copy_options::directories_only | std::filesystem::copy_options::skip_symlinks;
         std::filesystem::copy( m_src_path, m_new_path, opts );
      }

      void backup()
      {
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
         FILEZ_STDOUT( "Found files: " << m_old_files.size() );
      }

   private:
      std::size_t m_limit = 4000;
      std::size_t m_empty_files = 0;
      std::size_t m_copied_files = 0;
      std::size_t m_copied_bytes = 0;
      std::size_t m_linked_files = 0;
      std::size_t m_linked_bytes = 0;

      const file_info_by_path_set m_src_files;
      const file_info_by_size_map m_old_files;

      void backup( file_info& fi )
      {
         const auto to = transfer( fi.path(), m_src_path, m_new_path );

         if( fi.stat().size() == 0 ) {
            backup_empty( to );
         }
         else if( fi.stat().size() < m_limit ) {
            backup_copy( fi, to );
         }
         else if( !backup_link( fi, to ) ) {
            backup_copy( fi, to );
         }
      }

      void backup_empty( const std::filesystem::path& to )
      {
         create_file( to );
         ++m_empty_files;
         FILEZ_STDOUT( "Create: " << to );
      }

      [[nodiscard]] bool backup_link( file_info& fi, const std::filesystem::path& to )
      {
         const auto iter = m_old_files.find( fi.stat().size() );

         if( iter == m_old_files.end() ) {
            return false;
         }
         for( const auto& of : iter->second ) {
            if( of->smart_hash() == fi.smart_hash() ) {
               constexpr auto opts = std::filesystem::copy_options::create_hard_links;

               if( !std::filesystem::copy_file( of->path(), to, opts ) ) {
                  FILEZ_ERROR( "TODO (hard link old backup file failed" );
               }
               ++m_linked_files;
               m_linked_bytes += fi.stat().size();
               FILEZ_STDOUT( "Link: " << of->path() << " -> " << to );
               return true;
            }
         }
         return false;
      }

      void backup_copy( file_info& fi, const std::filesystem::path& to )
      {
         if( !std::filesystem::copy_file( fi.path(), to ) ) {
            FILEZ_ERROR( "TODO (copy source file failed)" );
         }
         ++m_copied_files;
         m_copied_bytes += fi.stat().size();
         FILEZ_STDOUT( "Copy: " << fi.path() << " -> " << to );
      }
   };

}  // namespace filez
