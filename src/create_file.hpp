// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <unistd.h>

#include <sys/types.h>

#include "macros.hpp"

namespace filez
{
   // Not quite sure why std::filesystem doesn't provide anything that can do this
   // (and I don't like using a std::ofstream just to create an empty file, sorry.)

   inline void create_file( const std::filesystem::path& path )
   {
      const int fd = ::open( path.c_str(), O_WRONLY | O_CREAT | O_EXCL );

      if( fd < 0 ) {
         FILEZ_ERRNO( "unable to create() path [ "<< path << " ] as empty file" );
      }
      ::close( fd );
   }

} // filez
