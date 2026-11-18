// Copyright (c) 2022-2026 Dr. Colin Hirsch - All Rights Reserved

#undef NDEBUG

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "arguments.hpp"

namespace
{
   [[nodiscard]] std::optional< std::string > parse( filez::arguments& args, std::vector< std::string > values )
   {
      values.insert( values.begin(), "args_unit_test" );
      std::vector< char* > argv;
      for( auto& value : values ) {
         argv.push_back( value.data() );
      }
      argv.push_back( nullptr );

      std::ostringstream error;
      auto* previous = std::cerr.rdbuf( error.rdbuf() );
      const bool success = args.parse_nothrow( static_cast< int >( values.size() ), argv.data() );
      std::cerr.rdbuf( previous );
      if( success ) {
         assert( error.str().empty() );
         return std::nullopt;
      }
      return error.str();
   }

   void test_no_arguments()
   {
      std::vector< std::filesystem::path > paths;
      filez::arguments args( paths );
      bool flag = false;
      std::size_t size = 123;
      args.add_bool( 'a', flag );
      args.add_size( 'c', size );

      const auto result = parse( args, {} );
      assert( !result );
      assert( paths.empty() );
      assert( !flag );
      assert( size == 123 );
   }

   void test_paths()
   {
      std::vector< std::filesystem::path > paths{ "existing" };
      filez::arguments args( paths );
      const auto result = parse( args, { "first", "", "with spaces", "../relative", "/absolute", "--", "-", "-a", "--", "--unknown" } );
      const std::vector< std::filesystem::path > expected{ "existing", "first", "", "with spaces", "../relative", "/absolute", "-", "-a", "--", "--unknown" };
      assert( !result );
      assert( paths == expected );
   }

   void test_flags_between_paths()
   {
      std::vector< std::filesystem::path > paths;
      filez::arguments args( paths );
      bool enabled = false;
      bool recursive = true;
      args.add_bool( 'a', enabled );
      args.add_bool( 'R', recursive );

      const auto result = parse( args, { "first", "-aR", "second" } );
      const std::vector< std::filesystem::path > expected{ "first", "second" };
      assert( !result );
      assert( enabled );
      assert( !recursive );
      assert( paths == expected );
   }

   void test_end_of_options()
   {
      const std::vector< std::vector< std::string > > cases{ { "--" }, { "--", "-a", "-c", "--" } };
      for( const auto& values : cases ) {
         std::vector< std::filesystem::path > paths;
         filez::arguments args( paths );
         bool flag = false;
         std::size_t size = 123;
         args.add_bool( 'a', flag );
         args.add_size( 'c', size );

         const auto result = parse( args, values );
         const std::vector< std::filesystem::path > expected( values.begin() + 1, values.end() );
         assert( !result );
         assert( !flag );
         assert( size == 123 );
         assert( paths == expected );
      }
   }

   void test_callback_order()
   {
      struct test_case
      {
         std::vector< std::string > values;
         std::string_view order;
      };
      const std::vector< test_case > cases{
         { { "-hH" }, "hH" },
         { { "-Hh" }, "Hh" },
         { { "-h", "-H" }, "hH" },
         { { "-H", "-h" }, "Hh" }
      };
      for( const auto& test : cases ) {
         std::vector< std::filesystem::path > paths;
         filez::arguments args( paths );
         std::string calls;
         args.add_bool( 'h', [ &calls ]( const std::string_view value ){
            assert( value == "h" );
            calls += 'h';
         } );
         args.add_bool( 'H', [ &calls ]( const std::string_view value ){
            assert( value == "H" );
            calls += 'H';
         } );

         const auto result = parse( args, test.values );
         assert( !result );
         assert( calls == test.order );
         assert( paths.empty() );
      }
   }

   void test_sizes()
   {
      const auto maximum = std::numeric_limits< std::size_t >::max();
      const std::vector< std::pair< std::string, std::size_t > > cases{ { "0", 0 }, { "00042", 42 }, { std::to_string( maximum ), maximum } };
      for( const auto& [ value, expected_size ] : cases ) {
         std::vector< std::filesystem::path > paths;
         filez::arguments args( paths );
         bool flag = false;
         std::size_t size = 123;
         args.add_bool( 'a', flag );
         args.add_size( 'c', size );

         const auto result = parse( args, { "source", "-ac", value, "destination" } );
         const std::vector< std::filesystem::path > expected{ "source", "destination" };
         assert( !result );
         assert( flag );
         assert( size == expected_size );
         assert( paths == expected );
      }
   }

   void test_size_followed_by_flag()
   {
      std::vector< std::filesystem::path > paths;
      filez::arguments args( paths );
      bool flag = false;
      std::size_t size = 123;
      args.add_bool( 'a', flag );
      args.add_size( 'c', size );

      const auto result = parse( args, { "-c", "42", "-a", "file" } );
      const std::vector< std::filesystem::path > expected{ "file" };
      assert( !result );
      assert( size == 42 );
      assert( flag );
      assert( paths == expected );
   }

   void test_invalid_sizes()
   {
      const std::vector< std::string > values{ "", "-1", "+1", " 1", "1 ", "1x", "0x10", "--", std::to_string( std::numeric_limits< std::size_t >::max() ) + "0" };
      for( const auto& value : values ) {
         std::vector< std::filesystem::path > paths;
         filez::arguments args( paths );
         std::size_t size = 123;
         args.add_size( 'c', size );

         const auto result = parse( args, { "-c", value } );
         assert( result );
         assert( result->find( "option '-c'" ) != std::string::npos );
         assert( result->find( "nonnegative decimal byte count" ) != std::string::npos );
         assert( size == 123 );
         assert( paths.empty() );
      }
   }

   void test_option_errors()
   {
      struct test_case
      {
         std::vector< std::string > values;
         std::string_view error;
      };
      const std::vector< test_case > cases{
         { { "-q" }, "unknown option '-q'" },
         { { "--help" }, "unknown option '--help'" },
         { { "--size=1" }, "unknown option '--size=1'" },
         { { "-" }, "unknown option '-'" },
         { { "-aa" }, "must not be repeated" },
         { { "-a", "-a" }, "must not be repeated" },
         { { "-h", "-h" }, "must not be repeated" },
         { { "-c", "1", "-c", "2" }, "must not be repeated" },
         { { "-c" }, "requires a parameter" },
         { { "-ac" }, "requires a parameter" },
         { { "-ca", "1" }, "must be last" },
         { { "-c10" }, "must be last" }
      };
      for( const auto& test : cases ) {
         std::vector< std::filesystem::path > paths;
         filez::arguments args( paths );
         bool flag = false;
         std::size_t size = 123;
         args.add_bool( 'a', flag );
         args.add_bool( 'h', []( const std::string_view ){} );
         args.add_size( 'c', size );

         const auto result = parse( args, test.values );
         assert( result );
         assert( result->find( "command line argument error:" ) != std::string::npos );
         assert( result->find( test.error ) != std::string::npos );
      }
   }

   void test_callback_exception()
   {
      std::vector< std::filesystem::path > paths;
      filez::arguments args( paths );
      args.add_bool( 'a', []( const std::string_view ){ throw std::runtime_error( "callback failed" ); } );

      const auto result = parse( args, { "-a" } );
      assert( result );
      assert( result->find( "callback failed" ) != std::string::npos );
   }
}

int main()
{
   test_no_arguments();
   test_paths();
   test_flags_between_paths();
   test_end_of_options();
   test_callback_order();
   test_sizes();
   test_size_followed_by_flag();
   test_invalid_sizes();
   test_option_errors();
   test_callback_exception();
   return 0;
}
