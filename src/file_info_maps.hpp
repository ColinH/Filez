// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "file_info.hpp"
#include "file_stat.hpp"

namespace filez
{
   using file_info_by_node_map = std::map< file_node, std::vector< std::shared_ptr< file_info > > >;
   using file_info_by_size_map = std::map< std::size_t, std::vector< std::shared_ptr< file_info > > >;

   template< typename S, typename I >
   void make_file_info_by_node_map_impl( S& result, const std::filesystem::path& path )
   {
      for( const auto& de : I( path ) ) {
         const auto fi = std::make_shared< file_info >( de.path() );
         if( fi->stat().is_file() ) {
            result.try_emplace( fi->stat().node() ).first->second.emplace_back( fi );
         }
      }
   }

   template< typename S, typename I >
   void make_file_info_by_size_map_impl( S& result, const std::filesystem::path& path )
   {
      for( const auto& de : I( path ) ) {
         const auto fi = std::make_shared< file_info >( de.path() );
         if( fi->stat().is_file() ) {
            result.try_emplace( fi->stat().size() ).first->second.emplace_back( fi );
         }
      }
   }

   template< typename S, typename I >
   [[nodiscard]] S make_file_info_by_node_map_impl( const std::filesystem::path& path )
   {
      S result;
      make_file_info_by_node_map_impl< S, I >( result, path );
      return result;
   }

   template< typename S, typename I >
   [[nodiscard]] S make_file_info_by_size_map_impl( const std::filesystem::path& path )
   {
      S result;
      make_file_info_by_size_map_impl< S, I >( result, path );
      return result;
   }

   [[nodiscard]] inline file_info_by_node_map make_file_info_by_node_map( const std::filesystem::path& path )
   {
      return make_file_info_by_node_map_impl< file_info_by_node_map, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_info_by_node_map make_full_file_info_by_node_map( const std::filesystem::path& path )
   {
      return make_file_info_by_node_map_impl< file_info_by_node_map, std::filesystem::recursive_directory_iterator >( path );
   }

   [[nodiscard]] inline file_info_by_size_map make_file_info_by_size_map( const std::filesystem::path& path )
   {
      return make_file_info_by_size_map_impl< file_info_by_size_map, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_info_by_size_map make_full_file_info_by_size_map( const std::filesystem::path& path )
   {
      return make_file_info_by_size_map_impl< file_info_by_size_map, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
