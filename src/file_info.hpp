// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <utility>

#include "file_stat.hpp"
#include "hash_file.hpp"

namespace filez
{
   class file_info
   {
   public:
      explicit file_info( std::filesystem::path&& path )
         : m_path( std::move( path ) )
      {}

      explicit file_info( const std::filesystem::path& path )
         : m_path( path )
      {}

      [[nodiscard]] const std::filesystem::path& path() const noexcept
      {
         return m_path;
      }

      [[nodiscard]] const file_stat& stat()
      {
         if( !m_stat.is_valid() ) {
            m_stat.update( m_path );
         }
         return m_stat;
      }

      [[nodiscard]] const std::string& smart_hash()
      {
         if( m_smart_hash.empty() ) {
            const file_open open( m_path );
            m_smart_hash = hash_file_smart( m_path, open, stat() );
         }
         return m_smart_hash;
      }

      [[nodiscard]] const std::string& total_hash()
      {
         if( m_total_hash.empty() ) {
            if( ( !m_smart_hash.empty() ) && ( m_smart_hash[ 0 ] != 'P' ) ) {
               m_total_hash = m_smart_hash;
            }
            else {
               const file_open open( m_path );
               m_total_hash = hash_file_total( m_path, open, stat() );
            }
         }
         return m_total_hash;
      }

   private:
      std::filesystem::path m_path;

      file_stat m_stat;

      std::string m_smart_hash;
      std::string m_total_hash;
   };

}  // namespace filez
