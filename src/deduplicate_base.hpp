// Copyright (c) 2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <string>

#include "file_info_maps.hpp"
#include "file_stat.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class deduplicate_base
   {
   protected:
      deduplicate_base( const std::filesystem::path& source_dir, const std::filesystem::path& merged_dir )
         : m_src_path( std::filesystem::canonical( source_dir ) ),
           m_new_path( initialize_new_path( merged_dir ) ),
           m_src_stat( m_src_path ),
           m_new_stat( m_new_path ),
           m_src_files( make_full_file_info_by_size_map( m_src_path ) )
      {
         if( !m_src_stat.is_dir() ) {
            FILEZ_ERROR( "source path " << m_src_path << " is not a directory" );
         }
         if( m_src_stat.device() != m_new_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and merged dir " << m_new_path << " are not on the same filesystem" );
         }
         if( !independent( m_src_path, m_new_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and merged dir " << m_new_path << " are not independent" );
         }
      }

      const std::filesystem::path m_src_path;
      const std::filesystem::path m_new_path;

      const file_stat m_src_stat;
      const file_stat m_new_stat;

      const file_info_by_size_map m_src_files;

   private:
      [[nodiscard]] std::filesystem::path initialize_new_path( const std::filesystem::path& merged_dir )
      {
         if( std::filesystem::exists( merged_dir ) ) {
            FILEZ_ERROR( "merged dir path " << merged_dir << " must not exist yet" );
         }
         if( !std::filesystem::create_directory( merged_dir ) ) {
            FILEZ_ERROR( "unable to create directory for merged dir " << merged_dir );
         }
         return std::filesystem::canonical( merged_dir );
      }
   };

}  // namespace filez
