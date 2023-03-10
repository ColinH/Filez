// Copyright (c) 2023 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "deduplicate_work.hpp"
#include "macros.hpp"

std::vector< std::filesystem::path > paths;

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   filez::deduplicate_args fia;

   args.add_bool( 'h', fia.h );
   args.add_bool( 'H', fia.H );
   args.add_size( 'c', fia.c );

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() != 2 ) || ( !fia.valid() ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " [options]... <source_dir> <merged_dir>" );
      FILEZ_STDERR( "  Creates a new directory hierarchy under merged_dir that mirrors source_dir" );
      FILEZ_STDERR( "  where all files from source_dir are hard linked into merged_dir such that" );
      FILEZ_STDERR( "  all identical copies of a file in source_dir are hard linked to one file." );
      FILEZ_STDERR( "  Files in the source dir are considered identical when..." );
      FILEZ_STDERR( "    -h   the file size and smart hash match." );
      FILEZ_STDERR( "    -H   the file size and total hash match." );
      FILEZ_STDERR( "    -c N Copy instead of hard link all files smaller than N, default 0." );
      FILEZ_STDERR( "  The source and merged dir must reside on the same filesystem and merged dir must not exist yet." );
      FILEZ_STDERR( "  Exactly one of -h and -H must be given." );
      FILEZ_STDERR( "Copyright (c) 2023 Dr. Colin Hirsch" );
      return 1;
   }
   filez::deduplicate_work( paths.front(), paths.back(), fia ).merge();
   return 0;
}
