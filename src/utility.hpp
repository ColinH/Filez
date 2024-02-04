// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <algorithm>
#include <filesystem>

#include "macros.hpp"

namespace filez
{
   [[nodiscard]] inline bool ignore( const std::filesystem::path& path ) noexcept
   {
      if( path.empty() ) {
         return true;
      }
      const auto& n = path.native();
      // TODO: Extension/configuration point.
      return ( n == "." ) || ( n == ".." );
   }

   [[nodiscard]] inline std::size_t components( const std::filesystem::path& path ) noexcept
   {
      return std::distance( path.begin(), path.end() );
   }

   [[nodiscard]] inline bool independent( const std::filesystem::path& left, const std::filesystem::path& right )
   {
      const auto l = std::filesystem::canonical( left );
      const auto r = std::filesystem::canonical( right );

      if( l == r ) {
         return false;
      }
      if( l.empty() || r.empty() ) {
         return false;
      }
      if( l.root_path() != r.root_path() ) {
         return true;
      }
      if( l.native().starts_with( r.native() ) ) {
         return ( components( l ) == components( r ) ) && ( l.filename() != r.filename() );
      }
      if( r.native().starts_with( l.native() ) ) {
         return ( components( l ) == components( r ) ) && ( l.filename() != r.filename() );
      }
      return true;
   }

   [[nodiscard]] inline bool transferred( const std::filesystem::path& source, const std::filesystem::path& original, const std::filesystem::path& candidate )
   {
      FILEZ_ASSERT( original.native().starts_with( source.native() ) );
      FILEZ_ASSERT( source.native().size() < original.native().size() );

      return candidate.native().ends_with( original.native().substr( source.native().size() ) );  // TODO: Use std::string_view instead of temporary std::string.
   }

   [[nodiscard]] inline std::filesystem::path transfer( const std::filesystem::path& path, const std::filesystem::path& left, const std::filesystem::path& right )
   {
      FILEZ_ASSERT( path != left );
      FILEZ_ASSERT( left != right );
      FILEZ_ASSERT( !path.empty() );
      FILEZ_ASSERT( !left.empty() );
      FILEZ_ASSERT( !right.empty() );
      FILEZ_ASSERT( path.native().starts_with( left.native() ) );

      const auto rel = path.native().substr( left.native().size() );

      FILEZ_ASSERT( !rel.empty() );
      FILEZ_ASSERT( rel[ 0 ] == '/' );

      return std::filesystem::path( right.native() + rel );
   }

}  // namespace filez
