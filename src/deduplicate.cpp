// Copyright (c) 2023-2025 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "deduplicate_args.hpp"
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
      FILEZ_STDERR( "  Creates a new directory hierarchy under merged_dir that mirrors source_dir." );
      FILEZ_STDERR( "  Directories are newly created. Files are hard-linked, not copied, such that" );
      FILEZ_STDERR( "  when source_dir contains multiple identical copies of a file then all of the" );
      FILEZ_STDERR( "  hard-linked versions in merged_dir will link to the same version of the file" );
      FILEZ_STDERR( "  in source_dir. Which version is chosen is unspecified." );
      FILEZ_STDERR( "  Files in the source dir are considered identical when..." );
      FILEZ_STDERR( "    -h   the file size and smart hash match." );
      FILEZ_STDERR( "    -H   the file size and total hash match." );
      FILEZ_STDERR( "    -c N Copy instead of hard link all files smaller than N bytes, default 0." );
      FILEZ_STDERR( "  Source and merged dir must be on the same filesystem. Merged dir must not exist." );
      FILEZ_STDERR( "  Exactly one of -h and -H must be given." );
      FILEZ_STDERR( "  The smart hash only hashes two or three small chunks" );
      FILEZ_STDERR( "    when the file is large and the extension is one for" );
      FILEZ_STDERR( "    which a partial hash is usually sufficient." );
      FILEZ_STDERR( "  The details are in hash_file.hpp and hash_size.hpp." );
      return 1;
   }
   filez::deduplicate_work( paths.front(), paths.back(), fia ).merge();
   return 0;
}
