// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <string>

#include <CommonCrypto/CommonDigest.h>

#include "hexdump.hpp"
#include "macros.hpp"

namespace filez
{
   class data_hash
   {
   public:
      data_hash() noexcept
      {
         CC_SHA256_Init( &m_hash );
      }

      data_hash( const void* data, const std::size_t size ) noexcept
         : data_hash()
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
            CC_SHA256_Update( &m_hash, data, size );
         }
      }

      [[nodiscard]] std::string result()
      {
         unsigned char tmp[ CC_SHA256_DIGEST_LENGTH ];
         CC_SHA256_Final( tmp, &m_hash );
         return hexdump( tmp, sizeof( tmp ) );
      }

      template< typename T >
      [[nodiscard]] std::string result( T&& data )
      {
         update( data );
         return result();
      }

      [[nodiscard]] std::string result( const void* data, const std::size_t size )
      {
         update( data, size );
         return result();
      }

   private:
      CC_SHA256_CTX m_hash;
   };

}  // namespace filez
