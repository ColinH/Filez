// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_list.hpp"
#include "macros.hpp"

namespace filez
{
   class node_name_variations
   {
   public:
      node_name_variations() noexcept = default;

      void add( const file_info_list& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( sp->stat().node() ).first->second.try_emplace( sp->path().filename() ).first->second.emplace_back( sp );
            }
         }
      }

      void work()
      {
         for( const auto& kv : m_map ) {
            FILEZ_ASSERT( !kv.second.empty() );

            if( kv.second.size() > 1 ) {
               FILEZ_STDOUT( kv.second.size() << " name variations for device " << kv.first.first << " inode " << kv.first.second );

               for( const auto& sv : kv.second ) {
                  for( const auto& fi : sv.second ) {
                     FILEZ_STDOUT( "   " << fi->path() );
                  }
               }
            }
         }
      }

   private:
      std::map< file_node, std::map< std::filesystem::path, std::vector< std::shared_ptr< file_info > > > > m_map;
   };

}  // namespace filez
