// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "incremental_work.hpp"
#include "macros.hpp"

std::vector< std::filesystem::path > paths;

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() < 2 ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " <source_dir> [old_backup]... <new_backup>" );
      FILEZ_STDERR( "  Creates a new directory hierarchy under new_backup that mirrors source_dir." );
      FILEZ_STDERR( "  Hard links files from the old_backups into new_backup when possible, copies" );
      FILEZ_STDERR( "  the files from source_dir when the file does not exist in a previous backup." );
      FILEZ_STDERR( "Copyright (c) 2022-2023 Dr. Colin Hirsch" );
      return 1;
   }
   filez::incremental_work incremental( paths.front(), paths.back() );

   for( std::size_t i = 1; i + 1 < paths.size(); ++i ) {
      incremental.add( paths[ i ] );
   }
   incremental.backup();
   return 0;
}
