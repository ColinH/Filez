// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "file_info_list.hpp"
#include "macros.hpp"

#include "find_variations.hpp"
#include "name_size_variations.hpp"
#include "name_smart_hash_variations.hpp"
#include "name_total_hash_variations.hpp"
#include "node_name_variations.hpp"
#include "smart_hash_name_variations.hpp"
#include "smart_hash_node_variations.hpp"
#include "total_hash_name_variations.hpp"
#include "total_hash_node_variations.hpp"

bool canonical = true;
bool recursive = true;

std::vector< std::filesystem::path > paths;

std::shared_ptr< filez::find_variations_base > finder = std::make_shared< filez::find_variations< filez::name_smart_hash_variations > >();

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   args.add_bool( 'C', canonical );
   args.add_bool( 'R', recursive );

   args.add_bool( 's', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::name_size_variations > >(); } );
   args.add_bool( 'i', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::node_name_variations > >(); } );

   args.add_bool( 'n', []( const std::string_view ){ /* finder = std::make_shared< filez::find_variations< filez::name_smart_hash_variations > >(); */ } );
   args.add_bool( 'N', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::name_total_hash_variations > >(); } );

   args.add_bool( 'h', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::smart_hash_name_variations > >(); } );
   args.add_bool( 'H', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::total_hash_node_variations > >(); } );

   args.add_bool( 'x', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::smart_hash_node_variations > >(); } );
   args.add_bool( 'X', []( const std::string_view ){ finder = std::make_shared< filez::find_variations< filez::total_hash_name_variations > >(); } );

   if( ( !args.parse_nothrow( argc, argv ) ) || paths.empty() ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " [OPTION]... DIRECTORY [DIRECTORY]..." );
      FILEZ_STDERR( "  Finds file meta data variations in one or more directories." );
      FILEZ_STDERR( "    -s   Finds variations of file size for the same file name." );
      FILEZ_STDERR( "    -i   Finds variations of file name for the same device and inode." );
      FILEZ_STDERR( "    -n   Finds variations of smart hash for the same file name (default)." );
      FILEZ_STDERR( "    -N   Finds variations of total hash for the same file name." );
      FILEZ_STDERR( "    -h   Finds variations of file name for the same smart hash." );
      FILEZ_STDERR( "    -H   Finds variations of file name for the same total hash." );
      FILEZ_STDERR( "    -x   Finds variations of device and inode for the same smart hash." );
      FILEZ_STDERR( "    -X   Finds variations of device and inode for the same total hash." );
      FILEZ_STDERR( "  Additional options are..." );
      FILEZ_STDERR( "    -R   to change to non-recursive scanning." );
      FILEZ_STDERR( "    -C   to disable normalising the given paths." );
      FILEZ_STDERR( "  Symbolic links are followed or ignored, can't remember." );
      FILEZ_STDERR( "  The smart hash only hashes two or three small chunks" );
      FILEZ_STDERR( "    when the file is large and the extension is one for" );
      FILEZ_STDERR( "    which a partial hash is usually sufficient." );
      FILEZ_STDERR( "  The details are in hash_file.hpp and hash_size.hpp." );
      FILEZ_STDERR( "Copyright (c) 2022-2023 Dr. Colin Hirsch" );
      return 1;
   }
   for( auto& path : paths ) {
      if( canonical ) {
         path = std::filesystem::canonical( path );
      }
      finder->add( recursive ? filez::make_full_file_info_list( path ) : filez::make_file_info_list( path ) );
   }
   finder->work();
   return 0;
}
