// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "macros.hpp"

namespace filez
{
   struct option
   {
      bool allowed = true;
      bool occurred = false;

      bool needs_parameter = false;
      bool can_be_repeated = false;

      option( std::function< void( std::string_view ) >&& callback )
         : callback( std::move( callback ) )
      {}

      option( const std::function< void( std::string_view ) >& callback )
         : callback( callback )
      {}

      std::function< void( std::string_view ) > callback;

      std::set< char > shorts;
      std::set< std::string, std::less<> > longs;
   };

   class arguments
   {
   public:
      template< typename F >
      explicit arguments( F&& f )
         : m_free( std::forward< F >( f ) )
      {}

      template< typename S >
      explicit arguments( std::set< S >& v )
         : m_free( [ & ]( const std::string_view s ){ v.emplace( s ); } )
      {}

      template< typename S >
      explicit arguments( std::vector< S >& v )
         : m_free( [ & ]( const std::string_view s ){ v.emplace_back( s ); } )
      {}

      void add_bool( const char c, bool& b )
      {
         add_impl( c, [ & ]( const std::string_view ){ b = !b; } );
      }

      void add_bool( const std::string_view s, bool& b )
      {
         add_impl( s, [ & ]( const std::string_view ){ b = !b; } );
      }

      void add_bool( const char c, std::function< void( std::string_view ) >&& callback )
      {
         add_impl( c, std::move( callback ) );
      }

      void add_bool( const char c, const std::function< void( std::string_view ) >& callback )
      {
         add_impl( c, callback );
      }

      void add_bool( const std::string_view s, std::function< void( std::string_view ) >&& callback )
      {
         add_impl( s, std::move( callback ) );
      }

      void add_bool( const std::string_view s, const std::function< void( std::string_view ) >& callback )
      {
         add_impl( s, callback );
      }

      void add_count( const char c, unsigned& n )
      {
         add_impl( c, [ & ]( const std::string_view ){ ++n; } )->can_be_repeated = true;
      }

      void add_count( const std::string_view s, unsigned& n )
      {
         add_impl( s, [ & ]( const std::string_view ){ ++n; } )->can_be_repeated = true;
      }

      // ...

      void add_string( const char c, std::string& x )
      {
         add_impl( c, [ & ]( const std::string_view v ){ x = v; } )->needs_parameter = true;
      }

      void add_string( const std::string_view s, std::string& x )
      {
         add_impl( s, [ & ]( const std::string_view v ){ x = v; } )->needs_parameter = true;
      }

      void add_string( const char c, std::string_view& x )
      {
         add_impl( c, [ & ]( const std::string_view v ){ x = v; } )->needs_parameter = true;
      }

      void add_string( const std::string_view s, std::string_view& x )
      {
         add_impl( s, [ & ]( const std::string_view v ){ x = v; } )->needs_parameter = true;
      }

      void add_string( const char c, std::function< void( std::string_view ) >&& callback )
      {
         add_impl( c, std::move( callback ) )->needs_parameter = true;
      }

      void add_string( const char c, const std::function< void( std::string_view ) >& callback )
      {
         add_impl( c, callback )->needs_parameter = true;
      }

      void add_string( const std::string_view s, std::function< void( std::string_view ) >&& callback )
      {
         add_impl( s, std::move( callback ) )->needs_parameter = true;
      }

      void add_string( const std::string_view s, const std::function< void( std::string_view ) >& callback )
      {
         add_impl( s, callback )->needs_parameter = true;
      }

      void add_strings( const char c, std::set< std::string >& x )
      {
         const auto o = add_impl( c, [ & ]( const std::string_view v ){ x.emplace( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const std::string_view s, std::set< std::string >& x )
      {
         const auto o = add_impl( s, [ & ]( const std::string_view v ){ x.emplace( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const char c, std::set< std::string_view >& x )
      {
         const auto o = add_impl( c, [ & ]( const std::string_view v ){ x.emplace( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const std::string_view s, std::set< std::string_view >& x )
      {
         const auto o = add_impl( s, [ & ]( const std::string_view v ){ x.emplace( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const char c, std::vector< std::string >& x )
      {
         const auto o = add_impl( c, [ & ]( const std::string_view v ){ x.emplace_back( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const std::string_view s, std::vector< std::string >& x )
      {
         const auto o = add_impl( s, [ & ]( const std::string_view v ){ x.emplace_back( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const char c, std::vector< std::string_view >& x )
      {
         const auto o = add_impl( c, [ & ]( const std::string_view v ){ x.emplace_back( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      void add_strings( const std::string_view s, std::vector< std::string_view >& x )
      {
         const auto o = add_impl( s, [ & ]( const std::string_view v ){ x.emplace_back( v ); } );
         o->needs_parameter = true;
         o->can_be_repeated = true;
      }

      // ...

      std::shared_ptr< option > add_impl( const char c, std::function< void( std::string_view ) >&& callback )
      {
         const auto p = m_shorts.try_emplace( c );
         FILEZ_ASSERT( p.second );
         p.first->second = std::make_shared< option >( std::move( callback ) );
         return p.first->second;
      }

      std::shared_ptr< option > add_impl( const char c, const std::function< void( std::string_view ) >& callback )
      {
         const auto p = m_shorts.try_emplace( c );
         FILEZ_ASSERT( p.second );
         p.first->second = std::make_shared< option >( callback );
         return p.first->second;
      }

      std::shared_ptr< option > add_impl( const std::string_view s, std::function< void( std::string_view ) >&& callback )
      {
         const auto p = m_longs.try_emplace( std::string( s ) );
         FILEZ_ASSERT( p.second );
         p.first->second = std::make_shared< option >( std::move( callback ) );
         return p.first->second;
      }

      std::shared_ptr< option > add_impl( const std::string_view s, const std::function< void( std::string_view ) >& callback )
      {
         const auto p = m_longs.try_emplace( std::string( s ) );
         FILEZ_ASSERT( p.second );
         p.first->second = std::make_shared< option >( callback );
         return p.first->second;
      }

      void add_option( const char c, std::shared_ptr< option >&& o )
      {
         const auto p = m_shorts.try_emplace( c, std::move( o ) );
         FILEZ_ASSERT( p.second );
      }

      void add_option( const char c, const std::shared_ptr< option >& o )
      {
         const auto p = m_shorts.try_emplace( c, o );
         FILEZ_ASSERT( p.second );
      }

      void add_option( const std::string_view s, std::shared_ptr< option >&& o )
      {
         const auto p = m_longs.try_emplace( std::string( s ), std::move( o ) );
         FILEZ_ASSERT( p.second );
      }

      void add_option( const std::string_view s, const std::shared_ptr< option >& o )
      {
         const auto p = m_longs.try_emplace( std::string( s ), o );
         FILEZ_ASSERT( p.second );
      }

      void incompatible( const std::string_view shorts )
      {
         for( const char c : shorts ) {
            option& o = *m_shorts.at( c );
            for( const char d : shorts ) {
               if( c != d ) {
                  o.shorts.emplace( d );
               }
            }
         }
      }

      void incompatible( const std::string_view left, const std::string_view right )
      {
         FILEZ_ASSERT( left != right );
         {
            auto i = m_longs.find( left );
            if( i != m_longs.end() ) {
               i->second->longs.emplace( right );
            }
         } {
            auto i = m_longs.find( right );
            if( i != m_longs.end() ) {
               i->second->longs.emplace( left );
            }
         }
         // m_longs.at( left )->longs.emplace( right );
         // m_longs.at( right )->longs.emplace( left );
      }

      void parse_throws( int argc, char** argv )
      {
         m_i = 1;
         m_argc = argc;
         m_argv = argv;

         loop();
      }

      [[nodiscard]] bool parse_nothrow( int argc, char** argv )
      {
         try {
            parse_throws( argc, argv );
            return true;
         }
         catch( const std::exception& e ) {
            FILEZ_STDERR( "command line argument error:" );
            FILEZ_STDERR( "   " << e.what() );
            return false;
         }
      }

   private:
      int m_i;
      int m_argc;
      char** m_argv;

      std::function< void( std::string_view ) > m_free;

      std::map< char, std::shared_ptr< option > > m_shorts;
      std::map< std::string, std::shared_ptr< option >, std::less<> > m_longs;

      void call( const char c, const bool last )
      {
         const auto o = m_shorts.find( c );

         if( o == m_shorts.end() ) {
            FILEZ_ERROR( "unknown option '-" << c << "'" );
         }
         if( !o->second->allowed ) {
            FILEZ_ERROR( "option '-" << c << "' incompatible with previous options" );
         }
         if( o->second->occurred && ( !o->second->can_be_repeated ) ) {
            FILEZ_ERROR( "option '-" << c << "' must not be repeated" );
         }
         if( !o->second->needs_parameter ) {
            o->second->occurred = true;
            o->second->callback( std::string_view( &c, 1 ) );
         }
         else if( !last ) {
            FILEZ_ERROR( "option '-" << c << "' requires a parameter but is not the last in '" << m_argv[ m_i ] << "'" );
         }
         else if( ++m_i >= m_argc ) {
            FILEZ_ERROR( "option '-" << c << "' requires a parameter but there ain't one following it" );
         }
         else {
            o->second->occurred = true;
            o->second->callback( std::string_view( m_argv[ m_i ] ) );
         }
         exclude( *o->second );
      }

      void call( const std::string_view s )
      {
         const auto o = m_longs.find( s );

         if( o == m_longs.end() ) {
            FILEZ_ERROR( "unknown option '--" << s << "'" );
         }
         if( !o->second->allowed ) {
            FILEZ_ERROR( "option '--" << s << "' incompatible with previous options" );
         }
         if( o->second->occurred && ( !o->second->can_be_repeated ) ) {
            FILEZ_ERROR( "option '--" << s << "' must not be repeated" );
         }
         if( !o->second->needs_parameter ) {
            o->second->occurred = true;
            o->second->callback( s );
         }
         else if( ++m_i >= m_argc ) {
            FILEZ_ERROR( "option '--" << s << "' requires a parameter but there ain't one following it" );
         }
         else {
            o->second->occurred = true;
            o->second->callback( std::string_view( m_argv[ m_i ] ) );
         }
         exclude( *o->second );
      }

      void call( const std::string_view s, const std::string_view p )
      {
         const auto o = m_longs.find( s );

         if( o == m_longs.end() ) {
            FILEZ_ERROR( "unknown option '--" << s << "'" );
         }
         if( !o->second->allowed ) {
            FILEZ_ERROR( "option '--" << s << "' incompatible with previous options" );
         }
         if( o->second->occurred && ( !o->second->can_be_repeated ) ) {
            FILEZ_ERROR( "option '--" << s << "' must not be repeated" );
         }
         if( !o->second->needs_parameter ) {
            FILEZ_ERROR( "option '--" << s << "' does not require a parameter but received '" << p << "'" );
         }
         else {
            o->second->occurred = true;
            o->second->callback( p );
         }
         exclude( *o->second );
      }

      void exclude( option& opt )
      {
         for( const char x : opt.shorts ) {
            m_shorts.at( x )->allowed = false;
         }
         for( const std::string& x : opt.longs ) {
            m_longs.at( x )->allowed = false;
         }
      }

      void loop()
      {
         for( bool options_disabled = false; m_i < m_argc; ++m_i ) {
            const std::string_view arg = m_argv[ m_i ];

            if( arg.empty() ) {
               m_free( arg );
            }
            else if( options_disabled || ( arg[ 0 ] != '-' ) ) {
               m_free( arg );
            }
            else if( arg.size() == 1 ) {
               call( '-', true );
            }
            else if( arg[ 1 ] != '-' ) {
               for( const char* p = m_argv[ m_i ] + 1; *p; ++p ) {
                  call( *p, p[ 1 ] == '\0' );
               }
            }
            else if( arg.size() == 2 ) {
               options_disabled = true;
            }
            else {
               const std::string_view view( arg.substr( 2 ) );
               const std::size_t equals = view.find( '=' );

               if( equals == std::string_view::npos ) {
                  call( arg );
               }
               else if( view.size() <= ( equals + 1 ) ) {
                  call( view.substr( 0, equals ), std::string_view() );
               }
               else {
                  call( view.substr( 0, equals ), view.substr( equals + 1 ) );
               }
            }
         }
      }
   };

}  // namespace filez
