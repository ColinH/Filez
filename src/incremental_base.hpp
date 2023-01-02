// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <string>

#include "file_info_list.hpp"
#include "file_stat.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class incremental_base
   {
   protected:
      incremental_base( const std::filesystem::path& source_dir, const std::filesystem::path old_backup, const std::filesystem::path new_backup )
         : m_src_path( std::filesystem::canonical( source_dir ) ),
           m_old_path( std::filesystem::canonical( old_backup ) ),
           m_new_path( initialize_new_path( new_backup ) )
      {
         const file_stat dir_stat( m_src_path );
         const file_stat old_stat( m_old_path );
         const file_stat new_stat( m_new_path );

         if( !dir_stat.is_dir() ) {
            FILEZ_ERROR( "source path " << m_src_path << " is not a directory" );
         }
         if( !old_stat.is_dir() ) {
            FILEZ_ERROR( "old backup path " << m_old_path << " is not a directory" );
         }
         if( dir_stat.device() == old_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and old backup " << m_old_path << " are on the same filesystem" );
         }
         if( dir_stat.device() == new_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and new backup " << m_new_path << " are on the same filesystem" );
         }
         if( old_stat.device() != new_stat.device() ) {
            FILEZ_ERROR( "old backup " << m_old_path << " and new backup " << m_new_path << " are not on the same filesystem" );
         }
         if( !independent( m_src_path, m_old_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and old backup " << m_old_path << " are not independent" );
         }
         if( !independent( m_src_path, m_new_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and new backup " << m_new_path << " are not independent" );
         }
         if( !independent( m_old_path, m_new_path ) ) {
            FILEZ_ERROR( "old backup " << m_old_path << " and new backup " << m_new_path << " are not independent" );
         }
      }

      const std::filesystem::path m_src_path;
      const std::filesystem::path m_old_path;
      const std::filesystem::path m_new_path;

   private:
      [[nodiscard]] std::filesystem::path initialize_new_path( const std::filesystem::path& new_backup )
      {
         if( std::filesystem::exists( new_backup ) ) {
            FILEZ_ERROR( "new backup path " << new_backup << " must not exist yet" );
         }
         if( !std::filesystem::create_directory( new_backup, m_old_path ) ) {
            FILEZ_ERROR( "unable to create directory for new backup " << new_backup );
         }
         return std::filesystem::canonical( new_backup );
      }
   };

}  // namespace filez
