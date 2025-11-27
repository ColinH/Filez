// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#define _DARWIN_USE_64_BIT_INODE 64

#include <cstdlib>
#include <unistd.h>

namespace filez
{
   [[nodiscard]] inline std::size_t get_pagesize_raw()
   {
      return ::sysconf( _SC_PAGESIZE );  // TODO: Check return value.
   }

   [[nodiscard]] inline std::size_t get_pagesize()
   {
      static const std::size_t pagesize = get_pagesize_raw();
      return pagesize;
   }

   [[nodiscard]] inline std::size_t checked_pagesize()
   {
      const std::size_t page = get_pagesize();
      FILEZ_ASSERT( ( page & ( page - 1 ) ) == 0 );
      return page;
   }

   [[nodiscard]] inline std::size_t rounded_up_to_pagesize( const std::size_t size )
   {
      static const std::size_t page = checked_pagesize();
      return ( size + page - 1 ) & ( -page );
   }

   [[nodiscard]] inline std::size_t rounded_down_to_pagesize( const std::size_t size )
   {
      static const std::size_t page = checked_pagesize();
      return size & ( -page );
   }

}  // namespace filez
