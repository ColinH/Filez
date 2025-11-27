// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_vector.hpp"
#include "macros.hpp"

namespace filez
{
   class smart_hash_size_duplicates
   {
   public:
      smart_hash_size_duplicates() noexcept = default;

      void add( const file_info_vector& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( sp->stat().size() ).first->second.emplace_back( sp );
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
                  map.try_emplace( fi->smart_hash() ).first->second.emplace_back( fi );
               }
               for( const auto& sv : map ) {
                  if( sv.second.size() > 1 ) {
                     FILEZ_STDOUT( sv.second.size() << " duplicates with same size " << kv.first << " and same smart hash" );

                     for( const auto& fi : sv.second ) {
                        FILEZ_STDOUT( "   " << fi->path() );
                     }
                  }
               }
            }
         }
      }

   private:
      std::map< std::size_t, std::vector< std::shared_ptr< file_info > > > m_map;
   };

}  // namespace filez
