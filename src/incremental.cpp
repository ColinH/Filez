// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "incremental_args.hpp"
#include "incremental_work.hpp"
#include "macros.hpp"

std::vector< std::filesystem::path > paths;

int main( int argc, char** argv )
{
   filez::arguments args( paths );
   filez::incremental_args fia;

   args.add_bool( 'h', fia.h );
   args.add_bool( 'H', fia.H );
   args.add_bool( 'n', fia.n );
   args.add_bool( 'N', fia.N );
   args.add_bool( 'p', fia.p );
   args.add_bool( 'P', fia.P );
   args.add_bool( 'x', fia.x );
   args.add_size( 'c', fia.c );

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() < 2 ) || ( !fia.valid() ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " [options]... <source_dir> [old_backup]... <new_backup>" );
      FILEZ_STDERR( "  Creates a new directory hierarchy under new_backup that mirrors source_dir." );
      FILEZ_STDERR( "  Hard links files from the old_backups into new_backup when possible, copies" );
      FILEZ_STDERR( "  the files from source_dir when the file does not exist in a previous backup." );
      FILEZ_STDERR( "  Files in the old backup(s) are hard linked instead of copied when..." );
      FILEZ_STDERR( "    -h   the file size and smart hash match." );
      FILEZ_STDERR( "    -H   the file size and total hash match." );
      FILEZ_STDERR( "    -n   the file size and file name and smart hash match." );
      FILEZ_STDERR( "    -N   the file size and file name and total hash match." );
      FILEZ_STDERR( "    -p   the file size and file name match." );
      FILEZ_STDERR( "    -P   the file size and relative path within source_dir and the old_backup dir match, including file name." );
      FILEZ_STDERR( "    -x   Consider freshly copied files as candidates for hard linking." );
      FILEZ_STDERR( "    -c N Copy instead of hard link all files smaller than N, default 0." );
      FILEZ_STDERR( "  Options can be combined, e.g. -hP (or -h -P) will search according to both -h and -P." );
      FILEZ_STDERR( "  At least one option different from -c must be given -- though everything except -x is a nop without an old_backup." );
      FILEZ_STDERR( "  The smart hash only hashes two or three small chunks" );
      FILEZ_STDERR( "    when the file is large and the extension is one for" );
      FILEZ_STDERR( "    which a partial hash is usually sufficient." );
      FILEZ_STDERR( "  The details are in hash_file.hpp and hash_size.hpp." );
      return 1;
   }
   filez::incremental_work incremental( paths.front(), paths.back(), fia );

   for( std::size_t i = 1; i + 1 < paths.size(); ++i ) {
      incremental.add( paths[ i ] );
   }
   incremental.backup();
   return 0;
}
