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
   class file_open
   {
   public:
      explicit file_open( const std::filesystem::path& path )
         : m_fd( ::open( path.c_str(), O_RDONLY ) )
      {
         if( m_fd < 0 ) {
            FILEZ_ERRNO( "unable to open() path [ "<< path << " ] for reading" );
         }
      }

      ~file_open()
      {
         if( m_fd >= 0 ) {
            ::close( m_fd );
         }
      }

      file_open( file_open&& ) = delete;
      file_open( const file_open& ) = delete;

      void operator= ( file_open&& ) = delete;
      void operator= ( const file_open& ) = delete;

      [[nodiscard]] int get() const noexcept
      {
         return m_fd;
      }

   private:
      int m_fd;
   };

} // filez
