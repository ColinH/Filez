// Copyright (c) 2024-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <filesystem>

#include "file_info_maps.hpp"
#include "file_stat.hpp"
#include "filesystem.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   class link_first_work
   {
   public:
      link_first_work( const std::filesystem::path& source_dir, const std::filesystem::path& sparse_dir )
         : m_src_path( std::filesystem::canonical( source_dir ) ),
           m_new_path( initialize_new_path( sparse_dir ) ),
           m_src_stat( m_src_path ),
           m_new_stat( m_new_path ),
           m_src_files( make_full_file_info_by_node_map( m_src_path ) )
      {
         if( !m_src_stat.is_dir() ) {
            FILEZ_ERROR( "source path " << m_src_path << " is not a directory" );
         }
         if( m_src_stat.device() != m_new_stat.device() ) {
            FILEZ_ERROR( "source " << m_src_path << " and sparse dir " << m_new_path << " are not on the same filesystem" );
         }
         if( !independent( m_src_path, m_new_path ) ) {
            FILEZ_ERROR( "source " << m_src_path << " and sparse dir " << m_new_path << " are not independent" );
         }
      }

      link_first_work( link_first_work&& ) = delete;
      link_first_work( const link_first_work& ) = delete;

      void operator=( link_first_work&& ) = delete;
      void operator=( const link_first_work& ) = delete;

      void perform()
      {
         std::size_t first = 0;
         std::size_t total = 0;

         for( const auto& pair : m_src_files ) {
            first += 1;
            total += pair.second.size();

            FILEZ_ASSERT( !pair.second.empty() );

            const std::filesystem::path& src = pair.second.front()->path();
            const std::filesystem::path dst = transfer( src, m_src_path, m_new_path );

            std::filesystem::create_directories( dst.parent_path() );

            hard_link_impl( src, dst );
         }
         FILEZ_STDOUT( "Total files found: " << total );
         FILEZ_STDOUT( "Total nodes found: " << first );
      }

   private:
      const std::filesystem::path m_src_path;
      const std::filesystem::path m_new_path;

      const file_stat m_src_stat;
      const file_stat m_new_stat;

      const file_info_by_node_map m_src_files;

      [[nodiscard]] std::filesystem::path initialize_new_path( const std::filesystem::path& sparse_dir )
      {
         if( std::filesystem::exists( sparse_dir ) ) {
            FILEZ_ERROR( "sparse dir path " << sparse_dir << " must not exist yet" );
         }
         if( !std::filesystem::create_directory( sparse_dir ) ) {
            FILEZ_ERROR( "unable to create directory for sparse dir " << sparse_dir );
         }
         return std::filesystem::canonical( sparse_dir );
      }
   };

}  // namespace filez
