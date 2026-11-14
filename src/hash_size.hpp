// Copyright (c) 2022-2026 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <map>
#include <string>

#include "system.hpp"

namespace filez
{
   [[nodiscard]] const auto& hash_size_data()
   {
      static const std::size_t media_hash_size = rounded_up_to_pagesize( 16384 );

      // NOTE: For obvious reasons (potentially) uncompressed file
      // formats like BMP and TAR should NEVER use a partial hash!

      static const std::map< std::string, std::size_t, decltype( []( const std::string& l, const std::string& r ){ return ::strcasecmp( l.c_str(), r.c_str() ) < 0; } ) > map = {
         { ".avi", media_hash_size },
         { ".mp4", media_hash_size },
         { ".m4v", media_hash_size },
         { ".mov", media_hash_size },
         { ".mpg", media_hash_size },
         { ".mpeg", media_hash_size },
         { ".mkv", media_hash_size },
         { ".flv", media_hash_size },
         { ".vob", media_hash_size },
         { ".wmv", media_hash_size },
         { ".swf", media_hash_size },
         { ".webm", media_hash_size },

         { ".aac", media_hash_size },
         { ".alac", media_hash_size },
         { ".flac", media_hash_size },
         { ".m4a", media_hash_size },
         { ".mp3", media_hash_size },
         { ".ogg", media_hash_size },
         { ".wma", media_hash_size },

         { ".cr2", media_hash_size },
         { ".dng", media_hash_size },
         { ".jpg", media_hash_size },
         { ".jpeg", media_hash_size },
         { ".nef", media_hash_size },
         { ".png", media_hash_size }
      };
      return map;
   }

   [[nodiscard]] std::size_t hash_size( const std::filesystem::path& path, const std::size_t size )
   {
      const auto& data = hash_size_data();
      const auto iter = data.find( path.extension().native() );
      return ( iter == data.end() ) ? size : iter->second;
   }

}  // namespace filez
