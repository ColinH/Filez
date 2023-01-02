// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstddef>
#include <filesystem>
#include <string>

#include "data_hash.hpp"
#include "file_mmap.hpp"
#include "file_open.hpp"
#include "file_stat.hpp"
#include "hash_size.hpp"
#include "system.hpp"

namespace filez
{
   // This cache is for when we find the same inode via multiple paths.

   class hash_cache
   {
   public:
      hash_cache() noexcept = default;

      [[nodiscard]] const std::string& get( const file_node node ) const noexcept
      {
         const auto iter = m_map.find( node );
         return ( iter == m_map.end() ) ? m_empty : iter->second;
      }

      [[nodiscard]] const std::string& put( const file_node node, std::string hash )
      {
         return m_map.try_emplace( node, std::move( hash ) ).first->second;
      }

   private:
      std::string m_empty;
      std::map< file_node, std::string > m_map;
   };

   // The first character of the result indicates the hash scope:
   // 'E' stands for "empty", i.e. the hashed file is empty.
   // 'T' stands for "total", i.e. all bytes of the file were hashed,
   // 'P' stands for "partial", i.e. that some bytes were skipped.
   // 'C' stands for "contents", i.e. the file contents are the hash.

   [[nodiscard]] inline std::string hash_file_total( const std::filesystem::path& path, const file_open& open, const file_stat& stat )
   {
      if( stat.size() == 0 ) {
         return "E";
      }
      static hash_cache cache;

      if( const std::string& hash = cache.get( stat.node() ); !hash.empty() ) {
         return hash;
      }
      file_mmap mmap( path, open, stat );

      if( stat.size() < 33 ) {
         return cache.put( stat.node(), "C" + mmap.string() );
      }
      data_hash hash;

      return cache.put( stat.node(), "T" + hash.result( mmap ) );
   }

   [[nodiscard]] inline std::string hash_file_smart( const std::filesystem::path& path, const file_open& open, const file_stat& stat )
   {
      if( stat.size() == 0 ) {
         return "E";
      }
      static hash_cache cache;

      if( const std::string& hash = cache.get( stat.node() ); !hash.empty() ) {
         return hash;
      }
      file_mmap mmap( path, open, stat );

      if( stat.size() < 33 ) {
         return cache.put( stat.node(), "C" + mmap.string() );
      }
      data_hash hash;

      const std::size_t size = hash_size( path, mmap.size() );

      // Small file or file without configured partial hash size: hash everything.

      if( mmap.size() <= 3 * size ) {
         return cache.put( stat.node(), "T" + hash.result( mmap ) );
      }
      // Large file with configured partial hash size: hash only two or three chunks:
      // Always the first and last chunk, for very large files also the "middle" one.

      hash.update( mmap.data(), size );

      if( mmap.size() > 1024 * size ) {
         const std::size_t offset = rounded_down_to_pagesize( mmap.size() / 2 );
         hash.update( mmap.data() + offset, size );
      } {
         const std::size_t offset = rounded_down_to_pagesize( mmap.size() - size );
         hash.update( mmap.data() + offset, mmap.size() - offset );
      }
      return cache.put( stat.node(), "P" + hash.result() );
   }

   [[nodiscard]] inline std::string hash_file_total( const std::filesystem::path& path )
   {
      const file_open open( path );
      const file_stat stat( path );
      return hash_file_total( path, open, stat );
   }

   [[nodiscard]] inline std::string hash_file_smart( const std::filesystem::path& path )
   {
      const file_open open( path );
      const file_stat stat( path );
      return hash_file_smart( path, open, stat );
   }

} // filez
