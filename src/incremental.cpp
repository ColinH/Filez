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

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() != 3 ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " <source_dir> <old_backup> <new_backup>" );
      FILEZ_STDERR( "Copyright (c) 2022-2023 Dr. Colin Hirsch" );
      return 1;
   }
   filez::incremental_work( paths[ 0 ], paths[ 1 ], paths[ 2 ] ).backup();
   return 0;
}
