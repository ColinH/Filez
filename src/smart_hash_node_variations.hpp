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
   class smart_hash_node_variations
   {
   public:
      smart_hash_node_variations() noexcept = default;

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
               if( std::count_if( kv.second.begin(), kv.second.end(), [ n = kv.second.front()->stat().node() ]( const auto& fi ){ return fi->stat().node() != n; } ) < 1 ) {
                  continue;  // All files of this size have the same device and inode so there can't be any variation and we don't need any hashes.
               }
               std::map< std::string, std::map< file_node, std::vector< std::shared_ptr< file_info > > > > map;

               for( const auto& fi : kv.second ) {
                  map.try_emplace( fi->smart_hash() ).first->second.try_emplace( fi->stat().node() ).first->second.emplace_back( fi );
               }
               for( const auto& sv : map ) {
                  if( sv.second.size() > 1 ) {
                     FILEZ_STDOUT( "SMART HASH NODE VARIATIONS" );
                     FILEZ_STDOUT( sv.second.size() << " node variations for smart hash" );

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
