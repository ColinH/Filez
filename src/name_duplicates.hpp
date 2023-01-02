// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "file_info_list.hpp"
#include "macros.hpp"

namespace filez
{
   class name_duplicates
   {
   public:
      name_duplicates() noexcept = default;

      void add( const file_info_list& list )
      {
         for( const auto& sp : list ) {
            if( sp->stat().is_file() ) {
               m_map.try_emplace( sp->path().filename() ).first->second.emplace_back( sp );
            }
         }
      }

      void work()
      {
         for( const auto& kv : m_map ) {
            FILEZ_ASSERT( !kv.second.empty() );

            if( kv.second.size() > 1 ) {
               FILEZ_STDOUT( kv.second.size() << " duplicates of file name " << kv.first );

               for( const auto& fi : kv.second ) {
                  FILEZ_STDOUT( "   " << fi->path() );
               }
            }
         }
      }

   private:
      std::map< std::filesystem::path, std::vector< std::shared_ptr< file_info > > > m_map;
   };

}  // namespace filez
