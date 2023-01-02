// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace filez
{
   template< typename T >
   [[nodiscard]] std::string hexdump( const T* begin, const T* const end )
   {
      static_assert( sizeof( T ) == 1 );

      std::string r;
      r.reserve( 2 * ( end - begin ) );

      static const char hex_chars[] = "0123456789abcdef";

      while( begin != end ) {
         r += hex_chars[ ( unsigned( * begin ) >> 4 ) & 0x0f ];
         r += hex_chars[   unsigned( * begin )        & 0x0f ];
         ++begin;
      }
      return r;
   }

   template< typename T >
   [[nodiscard]] std::string hexdump( const T* begin, const std::size_t length )
   {
      return hexdump( begin, begin + length );
   }

   [[nodiscard]] inline std::string hexdump( const std::string_view string )
   {
      return hexdump( string.data(), string.size() );
   }

}  // namespace filez
