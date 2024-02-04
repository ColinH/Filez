// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_vector.hpp"
#include "macros.hpp"

namespace filez
{
   class total_node_duplicates
   {
   public:
      total_node_duplicates() noexcept = default;

      void add( const file_info_vector& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( sp->stat().node() ).first->second.emplace_back( sp );
            }
         }
      }

      void work()
      {
         for( const auto& kv : m_map ) {
            FILEZ_ASSERT( !kv.second.empty() );

            if( ( kv.second.size() > 1 ) || ( kv.second.size() != kv.second.front()->stat().links() ) ) {
               FILEZ_STDOUT( kv.second.size() << " of " << kv.second.front()->stat().links() << " duplicates of device " << kv.first.first << " inode " << kv.first.second );

               for( const auto& fi : kv.second ) {
                  FILEZ_STDOUT( "   " << fi->path() );
               }
            }
         }
      }

   private:
      std::map< file_node, std::vector< std::shared_ptr< file_info > > > m_map;
   };

}  // namespace filez
