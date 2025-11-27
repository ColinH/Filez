// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_vector.hpp"
#include "macros.hpp"

namespace filez
{
   class name_total_hash_size_duplicates
   {
   public:
      name_total_hash_size_duplicates() noexcept = default;

      void add( const file_info_vector& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( std::make_pair( sp->stat().size(), sp->path().filename() ) ).first->second.emplace_back( sp );
            }
         }
      }

      void work()
      {
         for( const auto& kv : m_map ) {
            FILEZ_ASSERT( !kv.second.empty() );

            if( kv.second.size() > 1 ) {
               std::map< std::string, std::vector< std::shared_ptr< file_info > > > map;

               for( const auto& fi : kv.second ) {
                  map.try_emplace( fi->total_hash() ).first->second.emplace_back( fi );
               }
               for( const auto& sv : map ) {
                  if( sv.second.size() > 1 ) {
                     FILEZ_STDOUT( sv.second.size() << " duplicates of file name " << kv.first.second << " with same size " << kv.first.first << " and same total hash" );

                     for( const auto& fi : sv.second ) {
                        FILEZ_STDOUT( "   " << fi->path() );
                     }
                  }
               }
            }
         }
      }

   private:
      std::map< std::pair< std::size_t, std::filesystem::path >, std::vector< std::shared_ptr< file_info > > > m_map;
   };

}  // namespace filez
