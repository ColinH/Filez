// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <compare>
#include <cstring>
#include <filesystem>
#include <functional>
#include <unistd.h>
#include <utility>

#include <sys/stat.h>
#include <sys/types.h>

#include "file_open.hpp"
#include "macros.hpp"

namespace filez
{
   using file_time = __uint128_t;
   using file_node = std::pair< ::dev_t, ::ino_t >;

   class file_stat
   {
   public:
      file_stat() noexcept
      {
         std::memset( &m_file_stat, 0, sizeof( m_file_stat ) );
      }

      explicit file_stat( const std::filesystem::path& path )
      {
         update( path );
      }

      void update( const std::filesystem::path& path )
      {
         if( ::lstat( path.c_str(), &m_file_stat ) ) {
            FILEZ_ERRNO( "unable to lstat(2) path [ " << path << " ]" );
         }
         if( !same_user() ) {
            FILEZ_ERROR( "file [ " << path << " ] does not belong to user [ " << ::getuid() << " ]" );
         }
         FILEZ_ASSERT( is_valid() );
      }

      [[nodiscard]] bool is_valid() const noexcept
      {
         return links() > 0;  // TODO: Is this good enough or do we need to do The Right Thing and use std::optional< file_stat > instead of having a default c'tor?
      }

      [[nodiscard]] auto type() const noexcept
      {
         return m_file_stat.st_mode & S_IFMT;
      }

      [[nodiscard]] bool is_dir() const noexcept
      {
         return type() == S_IFDIR;
      }

      [[nodiscard]] bool is_file() const noexcept
      {
         return type() == S_IFREG;
      }

      [[nodiscard]] bool is_link() const noexcept
      {
         return type() == S_IFLNK;
      }

      [[nodiscard]] std::size_t size() const noexcept
      {
         return m_file_stat.st_size;
      }

      [[nodiscard]] std::size_t links() const noexcept
      {
         return m_file_stat.st_nlink;
      }

      // The C++17 filesystem library only contains a function that checks whether
      // two file stats refer to the same file, but it doesn't contain a function to
      // get at the underlying data which we need to stick in a std::map or similar:

      [[nodiscard]] file_node node() const noexcept
      {
         return { m_file_stat.st_dev, m_file_stat.st_ino };
      }

      [[nodiscard]] auto device() const noexcept
      {
         return m_file_stat.st_dev;
      }

      [[nodiscard]] auto inode() const noexcept
      {
         return m_file_stat.st_ino;
      }

   protected:
      struct ::stat m_file_stat;

      [[nodiscard]] bool same_user() const noexcept
      {
         return m_file_stat.st_uid == ::getuid();
      }
   };

   // [[nodiscard]] inline auto operator<=>( const file_stat& l, const file_stat& r ) noexcept
   // {
   //    return std::compare_three_way()( l.node(), r.node() );
   // }

   [[nodiscard]] bool operator<( const file_stat& l, const file_stat& r ) noexcept
   {
      return l.node() < r.node();
   }

   [[nodiscard]] bool operator==( const file_stat& l, const file_stat& r ) noexcept
   {
      return l.node() == r.node();
   }

} // filez
