// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <memory>
#include <set>
#include <string>

#include "file_info.hpp"
#include "macros.hpp"

namespace filez
{
   struct file_info_by_path_less
   {
      [[nodiscard]] bool operator()( const std::unique_ptr< file_info >& l, const std::unique_ptr< file_info >& r ) const noexcept
      {
         return l->path() < r->path();
      }
   };

   using file_info_by_path_set = std::set< std::unique_ptr< file_info >, file_info_by_path_less >;

   // GCC 12 doesn't like this: using file_info_by_path_set = std::set< std::unique_ptr< file_info >, decltype( []( const auto& l, const auto& r ){ return l->path() < r->path(); } ) >;

   template< typename S, typename I >
   [[nodiscard]] S make_file_info_set_impl( const std::filesystem::path& path )
   {
      S result;

      for( const auto& de : I( path ) ) {
         if( !result.emplace( std::make_unique< file_info >( de.path() ) ).second ) {
            FILEZ_ERROR( "duplicate file set entry " << de.path() );
         }
      }
      return result;
   }

   [[nodiscard]] inline file_info_by_path_set make_file_info_by_path_set( const std::filesystem::path& path )
   {
      return make_file_info_set_impl< file_info_by_path_set, std::filesystem::directory_iterator >( path );
   }

   [[nodiscard]] inline file_info_by_path_set make_full_file_info_by_path_set( const std::filesystem::path& path )
   {
      return make_file_info_set_impl< file_info_by_path_set, std::filesystem::recursive_directory_iterator >( path );
   }

}  // namespace filez
