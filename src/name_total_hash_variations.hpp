// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_vector.hpp"
#include "macros.hpp"

namespace filez
{
   class name_total_hash_variations
   {
   public:
      name_total_hash_variations() noexcept = default;

      void add( const file_info_vector& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( sp->path().filename() ).first->second.try_emplace( sp->total_hash() ).first->second.emplace_back( sp );
            }
         }
      }

      void work()
      {
         for( const auto& kv : m_map ) {
            FILEZ_ASSERT( !kv.second.empty() );

            if( kv.second.size() > 1 ) {
               FILEZ_STDOUT( kv.second.size() << " total hash variations for file name " << kv.first );

               for( const auto& sv : kv.second ) {
                  FILEZ_STDOUT( " hash group" );

                  for( const auto& fi : sv.second ) {
                     FILEZ_STDOUT( "   " << fi->path() );
                  }
               }
            }
         }
      }

   private:
      std::map< std::filesystem::path, std::map< std::string, std::vector< std::shared_ptr< file_info > > > > m_map;
   };

}  // namespace filez
