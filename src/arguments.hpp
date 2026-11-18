// Copyright (c) 2022-2026 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <charconv>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "macros.hpp"

namespace filez
{
   class arguments
   {
   public:
      explicit arguments( std::vector< std::filesystem::path >& paths ) noexcept
         : m_paths( paths )
      {}

      void add_bool( const char c, bool& b )
      {
         add_bool( c, [ &b ]( const std::string_view ){ b = !b; } );
      }

      void add_bool( const char c, std::function< void( std::string_view ) > callback )
      {
         add( c, { std::move( callback ) } );
      }

      void add_size( const char c, std::size_t& n )
      {
         add( c, { [ c, &n ]( const std::string_view value ){
            const auto end = value.data() + value.size();
            std::size_t size;
            const auto result = std::from_chars( value.data(), end, size );
            if( ( result.ec != std::errc{} ) || ( result.ptr != end ) ) {
               FILEZ_ERROR( "option '-" << c << "' requires a nonnegative decimal byte count, got '" << value << "'" );
            }
            n = size;
         }, true } );
      }

      [[nodiscard]] bool parse_nothrow( int argc, char** argv )
      {
         try {
            parse( argc, argv );
            return true;
         }
         catch( const std::exception& e ) {
            FILEZ_STDERR( "command line argument error:" );
            FILEZ_STDERR( "   " << e.what() );
            return false;
         }
      }

   private:
      struct option
      {
         std::function< void( std::string_view ) > callback;
         bool needs_parameter = false;
         bool occurred = false;
      };

      std::vector< std::filesystem::path >& m_paths;
      std::map< char, option > m_options;

      void add( const char c, option opt )
      {
         [[maybe_unused]] const bool inserted = m_options.try_emplace( c, std::move( opt ) ).second;
         FILEZ_ASSERT( inserted );
      }

      void parse( int argc, char** argv )
      {
         bool options_disabled = false;
         for( int i = 1; i < argc; ++i ) {
            const std::string_view arg = argv[ i ];
            if( options_disabled || !arg.starts_with( '-' ) ) {
               m_paths.emplace_back( arg );
               continue;
            }
            if( arg == "--" ) {
               options_disabled = true;
               continue;
            }
            if( ( arg.size() == 1 ) || arg.starts_with( "--" ) ) {
               FILEZ_ERROR( "unknown option '" << arg << "'" );
            }
            for( std::size_t pos = 1; pos < arg.size(); ++pos ) {
               const char c = arg[ pos ];
               const auto iter = m_options.find( c );
               if( iter == m_options.end() ) {
                  FILEZ_ERROR( "unknown option '-" << c << "'" );
               }
               auto& opt = iter->second;
               if( opt.occurred ) {
                  FILEZ_ERROR( "option '-" << c << "' must not be repeated" );
               }
               auto value = arg.substr( pos, 1 );
               if( opt.needs_parameter ) {
                  if( pos + 1 != arg.size() ) {
                     FILEZ_ERROR( "option '-" << c << "' must be last in '" << arg << "' to take a parameter" );
                  }
                  if( ++i == argc ) {
                     FILEZ_ERROR( "option '-" << c << "' requires a parameter" );
                  }
                  value = argv[ i ];
               }
               opt.occurred = true;
               opt.callback( value );
            }
         }
      }
   };

}  // namespace filez
