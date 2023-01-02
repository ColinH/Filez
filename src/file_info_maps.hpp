// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>

#include "file_info.hpp"

namespace filez
{
   using file_info_by_size_map = std::map< std::size_t, std::vector< std::shared_ptr< file_info > > >;

   template< typename S, typename I >
   [[nodiscard]] S make_file_info_by_size_map_impl( const std::filesystem::path& path )
   {
      S result;

      for( const auto& de : I( path ) ) {
         const auto fi = std::make_shared< file_info >( de.path() );
         if( fi->stat().is_file() ) {
            result.try_emplace( fi->stat().size() ).first->second.emplace_back( fi );
         }
      }
      return result;
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
