// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>

#include "file_path_set.hpp"
#include "file_stat.hpp"
#include "macros.hpp"
#include "utility.hpp"

namespace filez
{
   inline void tree_struct_diff( const std::filesystem::path& left_path, const std::filesystem::path& right_path, const bool check_sizes, const bool check_types )
   {
      const file_path_set left_set = make_file_path_set( left_path );
      const file_path_set right_set = make_file_path_set( right_path );

      // if( check_sizes && ( left_set.size() != right_set.size() ) ) {
      //    FILEZ_STDOUT( "Directory size  mismatch: " << left_set.size() << " and " << right_set.size() );
      // }
      auto left_iter = left_set.begin();
      auto right_iter = right_set.begin();

      while( ( left_iter != left_set.end() ) && ( right_iter != right_set.end() ) ) {
         const auto left_name = left_iter->filename();
         const auto right_name = right_iter->filename();

         if( ignore( left_name ) ) {
            ++left_iter;
            continue;
         }
         if( ignore( right_name ) ) {
            ++right_iter;
            continue;
         }
         if( left_name < right_name ) {
            FILEZ_STDOUT( " - " << *left_iter );
            ++left_iter;
            continue;
         }
         if( right_name < left_name ) {
            FILEZ_STDOUT( " + " << *right_iter );
            ++right_iter;
            continue;
         }
         const file_stat left_stat( *left_iter );
         const file_stat right_stat( *right_iter );

         if( check_types && ( left_stat.type() != right_stat.type() ) ) {
            FILEZ_STDOUT( "Type mismatch: " << *left_iter << " and " << *right_iter );
         }
         else if( check_sizes && left_stat.is_file() ) {
            if( left_stat.size() != right_stat.size() ) {
               FILEZ_STDOUT( "File size mismatch: " << *left_iter << " and " << *right_iter );
            }
         }
         else if( left_stat.is_dir() ) {
            tree_struct_diff( *left_iter, *right_iter, check_sizes, check_types );
         }
         ++left_iter;
         ++right_iter;
      }
      while( left_iter != left_set.end() ) {
         FILEZ_STDOUT( " - " << *left_iter );
         ++left_iter;
      }
      while( right_iter != right_set.end() ) {
         FILEZ_STDOUT( " + " << *right_iter );
         ++right_iter;
      }
   }

}  // namespace filez
