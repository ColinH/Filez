// Copyright (c) 2022-2026 Dr. Colin Hirsch - All Rights Reserved

#undef NDEBUG

#include <array>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>

#include "data_hash.hpp"

namespace
{
   constexpr std::string_view empty_digest = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
   constexpr std::string_view abc_digest = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

   void check_hash( const std::string_view data, const std::string_view expected )
   {
      filez::data_hash whole( data.data(), data.size() );
      assert( whole.result() == expected );

      // Every two-part split exercises buffering across update calls.
      for( std::size_t split = 0; split <= data.size(); ++split ) {
         filez::data_hash hash;
         hash.update( data.data(), split );
         hash.update( nullptr, 0 );
         hash.update( data.substr( split ) );
         assert( hash.result() == expected );
      }

      // Include updates smaller than, equal to, and larger than a SHA-256 block.
      for( const unsigned chunk_size : { 1U, 7U, 63U, 64U, 65U } ) {
         filez::data_hash hash;
         for( std::size_t offset = 0; offset < data.size(); offset += chunk_size ) {
            hash.update( data.substr( offset, chunk_size ) );
         }
         assert( hash.result() == expected );
      }
   }

   void test_known_vectors()
   {
      struct test_vector
      {
         std::string data;
         std::string_view expected;
      };
      const test_vector cases[]{
         { "", empty_digest },
         { "abc", abc_digest },
         { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
           "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1" },
         { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
           "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
           "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1" },
         // Padding and 64-byte block boundaries.
         { std::string( 55, 'a' ), "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318" },
         { std::string( 56, 'a' ), "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a" },
         { std::string( 57, 'a' ), "f13b2d724659eb3bf47f2dd6af1accc87b81f09f59f2b75e5c0bed6589dfe8c6" },
         { std::string( 63, 'a' ), "7d3e74a05d7db15bce4ad9ec0658ea98e3f06eeecf16b4c6fff2da457ddc2f34" },
         { std::string( 64, 'a' ), "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb" },
         { std::string( 65, 'a' ), "635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0" },
         { std::string( 119, 'a' ), "31eba51c313a5c08226adf18d4a359cfdfd8d2e816b13f4af952f7ea6584dcfb" },
         { std::string( 120, 'a' ), "2f3d335432c70b580af0e8e1b3674a7c020d683aa5f73aaaedfdc55af904c21c" },
         { std::string( 127, 'a' ), "c57e9278af78fa3cab38667bef4ce29d783787a2f731d4e12200270f0c32320a" },
         { std::string( 128, 'a' ), "6836cf13bac400e9105071cd6af47084dfacad4e5e302c94bfed24e013afb73e" },
         { std::string( 129, 'a' ), "c12cb024a2e5551cca0e08fce8f1c5e314555cc3fef6329ee994a3db752166ae" }
      };
      for( const auto& test : cases ) {
         check_hash( test.data, test.expected );
      }
   }

   void test_binary_data()
   {
      std::array< unsigned char, 256 > bytes;
      for( std::size_t i = 0; i < bytes.size(); ++i ) {
         bytes[ i ] = static_cast< unsigned char >( i );
      }
      constexpr std::string_view expected = "40aff2e9d2d8922e47afd4648e6967497158785fbd1da870e7110266bf944880";
      check_hash( { reinterpret_cast< const char* >( bytes.data() ), bytes.size() }, expected );

      filez::data_hash hash;
      hash.update( bytes );
      assert( hash.result() == expected );
   }

   void test_empty_updates()
   {
      filez::data_hash empty( nullptr, 0 );
      empty.update( nullptr, 0 );
      empty.update( std::string_view{} );
      assert( empty.result() == empty_digest );

      filez::data_hash nonempty( "abc", 3 );
      nonempty.update( nullptr, 0 );
      nonempty.update( std::string_view{} );
      assert( nonempty.result() == abc_digest );
   }

   void test_literal_updates()
   {
      filez::data_hash text;
      text.literal( "a" );
      text.literal( "" );
      text.literal( "bc" );
      assert( text.result() == abc_digest );

      filez::data_hash binary;
      binary.literal( "a\0b" );
      assert( binary.result() == "59b271ae1bbcb1d31d41929817f4b16fb439eb4f31520b5ad1d5ce98920a7138" );
   }

   void test_result_prefix()
   {
      for( const char prefix : { '\0', 'T', 'P' } ) {
         std::string expected( abc_digest );
         if( prefix != '\0' ) {
            expected.insert( expected.begin(), prefix );
         }
         filez::data_hash hash( "abc", 3 );
         assert( hash.result( prefix ) == expected );
      }
   }

   void test_long_message()
   {
      constexpr std::string_view expected = "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0";
      const std::string data( 1000000, 'a' );
      filez::data_hash whole( data.data(), data.size() );
      assert( whole.result() == expected );

      const std::string block( 1000, 'a' );
      filez::data_hash streamed;
      for( unsigned i = 0; i < 1000; ++i ) {
         streamed.update( block );
      }
      assert( streamed.result() == expected );
   }
}

int main()
{
   test_known_vectors();
   test_binary_data();
   test_empty_updates();
   test_literal_updates();
   test_result_prefix();
   test_long_message();
   return 0;
}
