// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <string>

#include "hexdump.hpp"
#include "macros.hpp"
#include "sha256.hpp"

namespace filez
{
   class data_hash
   {
   public:
      data_hash() noexcept = default;

      data_hash( const void* data, const std::size_t size ) noexcept
      {
         update( data, size );
      }

      data_hash( data_hash&& ) = delete;
      data_hash( const data_hash& ) = delete;

      void operator=( data_hash&& ) = delete;
      void operator=( const data_hash& ) = delete;

      template< unsigned N >
      void literal( const char( &s )[ N ] )
      {
         update( s, N - 1 );
      }

      template< typename T >
      void update( T&& data ) noexcept
      {
         static_assert( sizeof( *data.data() ) == 1 );
         update( data.data(), data.size() );
      }

      void update( const void* data, const std::size_t size ) noexcept
      {
         if( size > 0 ) {
            FILEZ_ASSERT( data );
            m_hash.update( data, size );
         }
      }

      [[nodiscard]] std::string result()
      {
         std::uint8_t tmp[ sha256_hash_size ];
         m_hash.finalise( tmp );
         return hexdump( tmp, sizeof( tmp ) );
      }

      [[nodiscard]] std::string result( const char c )
      {
         std::uint8_t tmp[ sha256_hash_size ];
         m_hash.finalise( tmp );
         return hexdump( c, tmp, sizeof( tmp ) );
      }

   private:
      sha256 m_hash;
   };

}  // namespace filez
