// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <set>

namespace filez
{
   using file_path_set = std::set< std::filesystem::path >;

   template< typename L, typename I >
   [[nodiscard]] inline file_path_set make_file_path_set_impl( const std::filesystem::path& path )
   {
      L result;

      for( const auto& de : I( path ) ) {
         result.emplace( de.path() );
      }
      return result;
   }

   [[nodiscard]] inline file_path_set make_file_path_set( const std::filesystem::path& path )
   {
      return make_file_path_set_impl< file_path_set, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_path_set make_full_file_path_set( const std::filesystem::path& path )
   {
      return make_file_path_set_impl< file_path_set, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
