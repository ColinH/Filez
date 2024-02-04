// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <unistd.h>

#include <sys/types.h>

#include "macros.hpp"

namespace filez
{
   inline void hard_link_impl( const std::filesystem::path& from, const std::filesystem::path& to )
   {
      errno = 0;
      if( ::link( from.c_str(), to.c_str() ) != 0 ) {
         FILEZ_ERRNO( "hard link " << from << " to " << to << " failed" );
      }
      // TODO: Is my libc++ broken or why does the following copy instead of creating hard links?
      // constexpr auto opts = std::filesystem::copy_options::create_hard_links;
      // if( !std::filesystem::copy_file( from, to, opts ) ) {
      //    FILEZ_ERROR( "hard link " << from << " to " << to << " failed" );
      // }
   }

   inline void copy_file_impl( const std::filesystem::path& from, const std::filesystem::path& to )
   {
      if( !std::filesystem::copy_file( from, to ) ) {
         FILEZ_ERROR( "copy file " << from << " to " << to << " failed" );
      }
   }

   // Not quite sure why std::filesystem doesn't provide anything that can do this
   // (and I don't like using a std::ofstream just to create an empty file, sorry.)

   inline void create_empty_file( const std::filesystem::path& path )
   {
      const int fd = ::open( path.c_str(), O_WRONLY | O_CREAT | O_EXCL );

      if( fd < 0 ) {
         FILEZ_ERRNO( "unable to create() path [ "<< path << " ] as empty file" );
      }
      ::close( fd );
   }

} // filez
