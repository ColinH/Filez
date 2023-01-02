// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

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
   [[nodiscard]] auto hash_size_data()
   {
      static const std::size_t audio_hash_size = rounded_up_to_pagesize( 4096 );
      static const std::size_t image_hash_size = rounded_up_to_pagesize( 4096 );
      static const std::size_t video_hash_size = rounded_up_to_pagesize( 16384 );
      static const std::size_t archive_hash_size = rounded_up_to_pagesize( 65536 );

      // NOTE: For obvious reasons (potentially) uncompressed file
      // formats like BMP and TAR should NEVER use a partial hash!

      static const std::map< std::string, std::size_t, decltype( []( const std::string& l, const std::string& r ){ return ::strcasecmp( l.c_str(), r.c_str() ) < 0; } ) > map = {
         { ".avi", video_hash_size },
         { ".mp4", video_hash_size },
         { ".m4v", video_hash_size },
         { ".mov", video_hash_size },
         { ".mpg", video_hash_size },
         { ".mpeg", video_hash_size },
         { ".mkv", video_hash_size },
         { ".flv", video_hash_size },
         { ".vob", video_hash_size },
         { ".wmv", video_hash_size },
         { ".swf", video_hash_size },
         { ".webm", video_hash_size },

         { ".aac", audio_hash_size },
         // { ".flac", audio_hash_size },
         { ".m4a", audio_hash_size },
         { ".mp3", audio_hash_size },
         { ".ogg", audio_hash_size },
         { ".wma", audio_hash_size },

         { ".cr2", image_hash_size },
         { ".dng", image_hash_size },
         // { ".gif", image_hash_size },
         { ".jpg", image_hash_size },
         { ".jpeg", image_hash_size },
         { ".nef", image_hash_size },
         { ".png", image_hash_size },

         { ".7z", archive_hash_size },
         { ".bz2", archive_hash_size },
         { ".gz", archive_hash_size },
         { ".lha", archive_hash_size },
         { ".lhx", archive_hash_size },
         { ".lzh", archive_hash_size },
         { ".rar", archive_hash_size },
         { ".tgz", archive_hash_size },
         { ".xpk", archive_hash_size },
         { ".xz", archive_hash_size },
         { ".zip", archive_hash_size }
      };
      return map;
   }

   [[nodiscard]] std::size_t hash_size( const std::filesystem::path& path, const std::size_t size )
   {
      const auto data = hash_size_data();
      const auto iter = data.find( path.extension().native() );
      return ( iter == data.end() ) ? size : iter->second;
   }

}  // namespace filez
