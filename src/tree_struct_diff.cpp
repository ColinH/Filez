// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>

#include "arguments.hpp"
#include "macros.hpp"
#include "tree_struct_diff.hpp"

bool canonical = true;

bool check_sizes = false;
bool check_types = false;

std::vector< std::filesystem::path > paths;

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   args.add_bool( 'C', canonical );
   args.add_bool( 's', check_sizes );
   args.add_bool( 't', check_types );

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() != 2 ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " [OPTION]... DIRECTORY DIRECTORY" );
      FILEZ_STDERR( "  Compares the structure of two directory trees comparing" );
      FILEZ_STDERR( "  the file names present or absent in each (sub-)directory." );
      FILEZ_STDERR( "  Options are..." );
      FILEZ_STDERR( "    -C   to disable normalising the given paths." );
      FILEZ_STDERR( "    -s   to also check file sizes for differences." );
      FILEZ_STDERR( "    -t   to also check file types for differences." );
      FILEZ_STDERR( "  File types are 'directory', 'file', etc." );
      return 1;
   }
   for( auto& path : paths ) {
      if( canonical ) {
         path = std::filesystem::canonical( path );
      }
   }
   filez::tree_struct_diff( paths[ 0 ], paths[ 1 ], check_sizes, check_types );
   return 0;
}
