// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <string>

#include "file_info_maps.hpp"
#include "file_info_sets.hpp"
#include "file_stat.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class incremental_base
   {
   public:
      void add( const std::filesystem::path& old_backup )
      {
         const auto old_path = std::filesystem::canonical( old_backup );
         const file_stat old_stat( old_path );

         FILEZ_STDOUT( "Scanning old backup " << old_path << "..." );

         if( !old_stat.is_dir() ) {
            FILEZ_ERROR( "old backup path " << old_path << " is not a directory" );
         }
         if( m_src_stat.device() == old_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and old backup " << old_path << " are on the same filesystem" );
         }
         if( old_stat.device() != m_new_stat.device() ) {
            FILEZ_ERROR( "old backup " << old_path << " and new backup " << m_new_path << " are not on the same filesystem" );
         }
         if( !independent( m_src_path, old_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and old backup " << old_path << " are not independent" );
         }
         if( !independent( old_path, m_new_path ) ) {
            FILEZ_ERROR( "old backup " << old_path << " and new backup " << m_new_path << " are not independent" );
         }
         make_file_info_by_size_map_impl< file_info_by_size_map, std::filesystem::recursive_directory_iterator >( m_old_files, old_path );
      }

   protected:
      incremental_base( const std::filesystem::path& source_dir, const std::filesystem::path& new_backup )
         : m_src_path( std::filesystem::canonical( source_dir ) ),
           m_new_path( initialize_new_path( new_backup ) ),
           m_src_stat( m_src_path ),
           m_new_stat( m_new_path ),
           m_src_files( make_full_file_info_by_path_set( m_src_path ) )
      {
         if( !m_src_stat.is_dir() ) {
            FILEZ_ERROR( "source path " << m_src_path << " is not a directory" );
         }
         if( m_src_stat.device() == m_new_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and new backup " << m_new_path << " are on the same filesystem" );
         }
         if( !independent( m_src_path, m_new_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and new backup " << m_new_path << " are not independent" );
         }
      }

      const std::filesystem::path m_src_path;
      const std::filesystem::path m_new_path;

      const file_stat m_src_stat;
      const file_stat m_new_stat;

      const file_info_by_path_set m_src_files;
      file_info_by_size_map m_old_files;

   private:
      [[nodiscard]] std::filesystem::path initialize_new_path( const std::filesystem::path& new_backup )
      {
         if( std::filesystem::exists( new_backup ) ) {
            FILEZ_ERROR( "new backup path " << new_backup << " must not exist yet" );
         }
         if( !std::filesystem::create_directory( new_backup ) ) {
            FILEZ_ERROR( "unable to create directory for new backup " << new_backup );
         }
         return std::filesystem::canonical( new_backup );
      }
   };

}  // namespace filez
