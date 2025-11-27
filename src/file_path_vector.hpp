// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <vector>

namespace filez
{
   using file_path_vector = std::vector< std::filesystem::path >;

   template< typename L, typename I >
   [[nodiscard]] inline file_path_vector make_file_path_vector_impl( const std::filesystem::path& path )
   {
      L result;

      for( const auto& de : I( path ) ) {
         result.emplace_back( de.path() );
      }
      return result;
   }

   [[nodiscard]] inline file_path_vector make_file_path_vector( const std::filesystem::path& path )
   {
      return make_file_path_vector_impl< file_path_vector, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_path_vector make_full_file_path_vector( const std::filesystem::path& path )
   {
      return make_file_path_vector_impl< file_path_vector, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
