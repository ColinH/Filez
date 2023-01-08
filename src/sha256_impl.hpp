//  Implementation of SHA256 hash function.
//  Original author: Tom St Denis, tomstdenis@gmail.com, http://libtom.org
//  Modified by WaterJuice retaining Public Domain license.
//  Modified by Dr. Colin Hirsch retaining Public Domain license.
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "macros.hpp"

namespace filez
{
   constexpr std::uint32_t sha256_K_impl[ 64 ] = {
      0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
      0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
      0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
      0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
      0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
      0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
      0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
      0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
      0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
      0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
      0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
      0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
      0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
   };

   [[nodiscard]] constexpr std::uint32_t sha256_Ch_impl( const std::uint32_t x, const std::uint32_t y, const std::uint32_t z ) noexcept
   {
      return z ^ ( x & ( y ^ z ) );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_Maj_impl( const std::uint32_t x, const std::uint32_t y, const std::uint32_t z ) noexcept
   {
      return ( ( x | y ) & z ) | ( x & y );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_ror_impl( const std::uint32_t value, const unsigned bits ) noexcept
   {
      return ( value >> bits ) | ( value << ( 32 - bits ) );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_S_impl( const std::uint32_t x, const std::uint32_t n ) noexcept
   {
      return sha256_ror_impl( x, n );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_R_impl( const std::uint32_t x, const std::uint32_t n ) noexcept
   {
      return x >> n;
   }

   [[nodiscard]] constexpr std::uint32_t sha256_Sigma0_impl( const std::uint32_t x ) noexcept
   {
      return sha256_S_impl( x, 2 ) ^ sha256_S_impl( x, 13 ) ^ sha256_S_impl( x, 22 );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_Sigma1_impl( const std::uint32_t x ) noexcept
   {
      return sha256_S_impl( x, 6 ) ^ sha256_S_impl( x, 11 ) ^ sha256_S_impl( x, 25 );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_Gamma0_impl( const std::uint32_t x ) noexcept
   {
      return sha256_S_impl( x, 7 ) ^ sha256_S_impl( x, 18 ) ^ sha256_R_impl( x, 3 );
   }

   [[nodiscard]] constexpr std::uint32_t sha256_Gamma1_impl( const std::uint32_t x ) noexcept
   {
      return sha256_S_impl( x, 17 ) ^ sha256_S_impl( x, 19 ) ^ sha256_R_impl( x, 10 );
   }

   inline sha256::sha256() noexcept
   {
      m_state[ 0 ] = 0x6A09E667UL;
      m_state[ 1 ] = 0xBB67AE85UL;
      m_state[ 2 ] = 0x3C6EF372UL;
      m_state[ 3 ] = 0xA54FF53AUL;
      m_state[ 4 ] = 0x510E527FUL;
      m_state[ 5 ] = 0x9B05688CUL;
      m_state[ 6 ] = 0x1F83D9ABUL;
      m_state[ 7 ] = 0x5BE0CD19UL;
   }

#define FILEZ_SHA256_LOAD32H(x, y)               \
   { x = ((std::uint32_t)((y)[0] & 255)<<24) |   \
         ((std::uint32_t)((y)[1] & 255)<<16) |   \
         ((std::uint32_t)((y)[2] & 255)<<8)  |   \
         ((std::uint32_t)((y)[3] & 255)); }

   inline void sha256::transform( const std::uint8_t* Buffer )
   {
      std::uint32_t S[ 8 ];

      for( unsigned i = 0; i < 8; ++i ) {
         S[ i ] = m_state[ i ];
      }
      std::uint32_t W[ 64 ];

      for( unsigned i = 0; i < 16; ++i ) {
         FILEZ_SHA256_LOAD32H( W[ i ], Buffer + ( 4 * i ) );
      }
      for( unsigned i=16; i < 64; ++i ) {
         W[ i ] = sha256_Gamma1_impl( W[ i - 2 ] ) + W[ i - 7 ] + sha256_Gamma0_impl( W[ i - 15 ] ) + W[ i - 16 ];
      }
      for( unsigned i = 0; i < 64; ++i ) {
         const std::uint32_t t0 = S[ 7 ] + sha256_Sigma1_impl( S[ 4 ] ) + sha256_Ch_impl( S[ 4 ], S[ 5 ], S[ 6 ] ) + sha256_K_impl[ i ] + W[ i ];
         const std::uint32_t t1 = sha256_Sigma0_impl( S[ 0 ] ) + sha256_Maj_impl( S[ 0 ], S[ 1 ], S[ 2 ] );
         S[ 3 ] += t0;
         S[ 7 ] = t0 + t1;
         std::rotate( S + 0, S + 7, S + 8 );
      }
      for( unsigned i = 0; i < 8; ++i ) {
         m_state[ i ] += S[ i ];
      }
   }

   inline void sha256::updatev( const std::uint8_t* Buffer, std::size_t BufferSize )
   {
      if( m_curlen > sizeof( m_buf ) ) {
         return;
      }
      while( BufferSize > 0 ) {
         if( ( m_curlen == 0 ) && ( BufferSize >= sha256_block_size ) ) {
            transform( Buffer );
            m_length += sha256_block_size * 8;
            Buffer += sha256_block_size;
            BufferSize -= sha256_block_size;
         }
         else {
            const std::size_t n = std::min( BufferSize, ( sha256_block_size - m_curlen ) );
            std::memcpy( m_buf + m_curlen, Buffer, n );
            m_curlen += n;
            Buffer += n;
            BufferSize -= n;

            if( m_curlen == sha256_block_size ) {
               transform( m_buf );
               m_length += 8 * sha256_block_size;
               m_curlen = 0;
            }
         }
      }
   }

#define FILEZ_SHA256_STORE32H( x, y )                                   \
   { (y)[0] = (std::uint8_t)(((x) >> 24) & 255);                        \
      (y)[1] = (std::uint8_t)(((x) >> 16) & 255);                       \
      (y)[2] = (std::uint8_t)(((x) >>  8) & 255);                       \
      (y)[3] = (std::uint8_t)( (x)        & 255); }

#define FILEZ_SHA256_STORE64H(x, y)                                     \
   { (y)[0] = (std::uint8_t)(((x)>>56)&255); (y)[1] = (std::uint8_t)(((x)>>48)&255); \
      (y)[2] = (std::uint8_t)(((x)>>40)&255); (y)[3] = (std::uint8_t)(((x)>>32)&255); \
      (y)[4] = (std::uint8_t)(((x)>>24)&255); (y)[5] = (std::uint8_t)(((x)>>16)&255); \
      (y)[6] = (std::uint8_t)(((x)>>8)&255); (y)[7] = (std::uint8_t)((x)&255); }

   inline void sha256::finalisev( std::uint8_t* Digest )
   {
      FILEZ_ASSERT( m_curlen < sizeof( m_buf ) );

      m_length += m_curlen * 8;
      m_buf[ m_curlen++ ] = 0x80;

      if( m_curlen > 56 ) {
         while( m_curlen < 64 ) {
            m_buf[ m_curlen++ ] = 0;
         }
         transform( m_buf );
         m_curlen = 0;
      }
      while( m_curlen < 56 ) {
         m_buf[ m_curlen++ ] = 0;
      }
      FILEZ_SHA256_STORE64H( m_length, m_buf + 56 );
      transform( m_buf );

      for( unsigned i = 0; i < 8; ++i ) {
         FILEZ_SHA256_STORE32H( m_state[ i ], Digest + ( 4 * i ) );
      }
   }

#undef FILEZ_SHA256_STORE32H
#undef FILEZ_SHA256_STORE64H

}  // namespace filez
