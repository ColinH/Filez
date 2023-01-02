// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "file_info_list.hpp"
#include "file_stat.hpp"
#include "macros.hpp"

namespace filez
{
   class smart_hash_name_variations
   {
   public:
      smart_hash_name_variations() noexcept = default;

      void add( const file_info_list& list )
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
               if( std::count_if( kv.second.begin(), kv.second.end(), [ n = kv.second.front()->path().filename() ]( const auto& fi ){ return fi->path().filename() != n; } ) < 1 ) {
                  continue;  // All files of this size have the same name so there can't be any variation and we don't need any hashes.
               }
               std::map< std::string, std::map< std::filesystem::path, std::vector< std::shared_ptr< file_info > > > > map;

               for( const auto& fi : kv.second ) {
                  map.try_emplace( fi->smart_hash() ).first->second.try_emplace( fi->path().filename() ).first->second.emplace_back( fi );
               }
               for( const auto& sv : map ) {
                  if( sv.second.size() > 1 ) {
                     FILEZ_STDOUT( "SMART HASH NAME VARIATIONS" );
                     FILEZ_STDOUT( sv.second.size() << " name variations for smart hash" );

                     std::size_t n = 0;
                     for( const auto& tv : sv.second ) {
                        FILEZ_STDOUT( "  variation " << ++n );
                        for( const auto& fi : tv.second ) {
                           FILEZ_STDOUT( "    " << fi->path() );
                        }
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
