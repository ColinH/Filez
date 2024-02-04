// Copyright (c) 2022-2024 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "file_info_vector.hpp"
#include "macros.hpp"

#include "find_duplicates.hpp"
#include "found_node_duplicates.hpp"
#include "name_duplicates.hpp"
#include "name_smart_hash_size_duplicates.hpp"
#include "name_total_hash_size_duplicates.hpp"
#include "name_size_duplicates.hpp"
#include "smart_hash_size_duplicates.hpp"
#include "total_hash_size_duplicates.hpp"
#include "total_node_duplicates.hpp"

bool canonical = true;
bool recursive = true;

std::vector< std::filesystem::path > paths;

std::shared_ptr< filez::find_duplicates_base > finder = std::make_shared< filez::find_duplicates< filez::smart_hash_size_duplicates > >();

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   args.add_bool( 'C', canonical );
   args.add_bool( 'R', recursive );

   args.add_bool( 'n', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::name_duplicates > >(); } );
   args.add_bool( 'N', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::name_size_duplicates > >(); } );

   args.add_bool( 'i', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::found_node_duplicates > >(); } );
   args.add_bool( 'I', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::total_node_duplicates > >(); } );

   args.add_bool( 'h', []( const std::string_view ){ /* finder = std::make_shared< filez::find_duplicates< filez::smart_hash_size_duplicates > >(); */ } );
   args.add_bool( 'H', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::total_hash_size_duplicates > >(); } );

   args.add_bool( 'x', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::name_smart_hash_size_duplicates > >(); } );
   args.add_bool( 'X', []( const std::string_view ){ finder = std::make_shared< filez::find_duplicates< filez::name_total_hash_size_duplicates > >(); } );

   if( ( !args.parse_nothrow( argc, argv ) ) || paths.empty() ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " [OPTION]... DIRECTORY [DIRECTORY]..." );
      FILEZ_STDERR( "  Finds duplicate files in one or more directories." );
      FILEZ_STDERR( "  Files are duplicates when they have the same..." );
      FILEZ_STDERR( "    -n   file name." );
      FILEZ_STDERR( "    -N   file name and size." );
      FILEZ_STDERR( "    -i   device and inode mode 1." );
      FILEZ_STDERR( "    -I   device and inode mode 2." );
      FILEZ_STDERR( "    -h   smart hash and size (default)." );
      FILEZ_STDERR( "    -H   total hash and size." );
      FILEZ_STDERR( "    -x   file name, smart hash and size." );
      FILEZ_STDERR( "    -X   file name, total hash and size." );
      FILEZ_STDERR( "  Additional options are..." );
      FILEZ_STDERR( "    -R   to change to non-recursive scanning." );
      FILEZ_STDERR( "    -C   to disable normalising the given paths." );
      FILEZ_STDERR( "  Special files like devices and pipes are ignored." );
      FILEZ_STDERR( "  The smart hash only hashes two or three small chunks" );
      FILEZ_STDERR( "    when the file is large and the extension is one for" );
      FILEZ_STDERR( "    which a partial hash is usually sufficient." );
      FILEZ_STDERR( "  The details are in hash_file.hpp and hash_size.hpp." );
      return 1;
   }
   for( auto& path : paths ) {
      if( canonical ) {
         path = std::filesystem::canonical( path );
      }
      finder->add( recursive ? filez::make_full_file_info_vector( path ) : filez::make_file_info_vector( path ) );
   }
   finder->work();
   return 0;
}
