// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>

#include <sys/mman.h>

#include "file_open.hpp"
#include "file_stat.hpp"
#include "macros.hpp"

namespace filez
{
   class file_mmap
   {
   public:
      explicit file_mmap( const std::filesystem::path& path )
         : file_mmap( path, file_open( path ), file_stat( path ) )
      {}

      file_mmap( const std::filesystem::path& path, const file_open& open, const file_stat& stat )
      {
         if( !stat.is_file() ) {
            FILEZ_ERROR( "unable to mmap() path " << path << " -- is not regular file" );
         }
         m_size = stat.size();

         if( m_size > 0 ) {
            errno = 0;
            m_data = static_cast< char * >( ::mmap( 0, m_size, PROT_READ, MAP_PRIVATE, open.get(), 0 ) );

            if( m_data == MAP_FAILED ) {
               FILEZ_ERRNO( "unable to mmap() path " << path );
            }
         }
      }

      ~file_mmap()
      {
         if( m_size ) {
            ::munmap( m_data, m_size );
         }
      }

      file_mmap( file_mmap&& ) = delete;
      file_mmap( const file_mmap& ) = delete;

      void operator=( file_mmap&& ) = delete;
      void operator=( const file_mmap& ) = delete;

      [[nodiscard]] std::size_t size() const noexcept
      {
         return m_size;
      }

      [[nodiscard]] const char* data() const noexcept
      {
         return m_data;
      }

      [[nodiscard]] char operator[]( const std::size_t o ) const noexcept
      {
         FILEZ_ASSERT( o < m_size );
         return m_data[ o ];
      }

      using iterator = const char *;
      using const_iterator = const char *;

      [[nodiscard]] iterator begin() const noexcept
      {
         return m_data;
      }

      [[nodiscard]] iterator end() const noexcept
      {
         return m_data + m_size;
      }

      [[nodiscard]] std::string string() const
      {
         return std::string( m_data, m_size );
      }

      [[nodiscard]] std::string_view string_view() const noexcept
      {
         return std::string_view( m_data, m_size );
      }

      void sequential() const
      {
         FILEZ_ASSERT( ::posix_madvise( m_data, m_size, POSIX_MADV_SEQUENTIAL ) == 0 );
         FILEZ_ASSERT( ::posix_madvise( m_data, m_size, POSIX_MADV_WILLNEED ) == 0 );
      }

   private:
      char* m_data = nullptr;
      std::size_t m_size = 0;
   };

} // filez
