// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "file_info.hpp"

namespace filez
{
   using file_info_vector = std::vector< std::shared_ptr< file_info > >;

   template< typename L, typename I >
   [[nodiscard]] inline file_info_vector make_file_info_vector_impl( const std::filesystem::path& path )
   {
      L result;

      for( const auto& de : I( path ) ) {
         result.emplace_back( std::make_shared< file_info >( de.path() ) );
      }
      return result;
   }

   [[nodiscard]] inline file_info_vector make_file_info_vector( const std::filesystem::path& path )
   {
      return make_file_info_vector_impl< file_info_vector, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_info_vector make_full_file_info_vector( const std::filesystem::path& path )
   {
      return make_file_info_vector_impl< file_info_vector, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
