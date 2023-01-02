// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <vector>

namespace filez
{
   using file_path_list = std::vector< std::filesystem::path >;

   template< typename L, typename I >
   [[nodiscard]] inline file_path_list make_file_path_list_impl( const std::filesystem::path& path )
   {
      L result;

      for( const auto& de : I( path ) ) {
         result.emplace_back( de.path() );
      }
      return result;
   }

   [[nodiscard]] inline file_path_list make_file_path_list( const std::filesystem::path& path )
   {
      return make_file_path_list_impl< file_path_list, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_path_list make_full_file_path_list( const std::filesystem::path& path )
   {
      return make_file_path_list_impl< file_path_list, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
