// Copyright (c) 2023-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstdint>

namespace filez
{
   constexpr std::size_t sha256_hash_size = 256 / 8;
   constexpr std::size_t sha256_block_size = 64;

   class sha256
   {
   public:
      sha256() noexcept;

      void update( const void* data, const std::size_t size )
      {
         updatev( static_cast< const std::uint8_t* >( data ), size );
      }

      void finalise( void* hash )
      {
         // hash MUST point to sha256_hash_size writable bytes!

         finalisev( static_cast< std::uint8_t* >( hash ) );
      }

   private:
      std::uint64_t m_length = 0;
      std::uint32_t m_state[ 8 ];
      std::uint32_t m_curlen = 0;
      std::uint8_t m_buf[ 64 ];

      void transform( const std::uint8_t* );
      void updatev( const std::uint8_t*, std::size_t );
      void finalisev( std::uint8_t* );
   };

}  // namespace filez

#include "sha256_impl.hpp"
