// Copyright (c) 2024 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <vector>

#include "arguments.hpp"
#include "link_first_work.hpp"
#include "macros.hpp"

std::vector< std::filesystem::path > paths;

int main( int argc, char** argv )
{
   filez::arguments args( paths );

   if( ( !args.parse_nothrow( argc, argv ) ) || ( paths.size() != 2 ) ) {
      FILEZ_STDERR( "Usage: " << argv[ 0 ] << " <source_dir> <sparse_dir>" );
      FILEZ_STDERR( "  Creates a new directory hierarchy under sparse_dir that partially mirrors" );
      FILEZ_STDERR( "  source_dir. Files under source_dir are hard-linked correspondingly into" );
      FILEZ_STDERR( "  sparse_dir only if no other hard-link to the same inode was already created" );
      FILEZ_STDERR( "  during the current operation, and directories are created under sparse_dir" );
      FILEZ_STDERR( "  as required by the hard-linked files. Which of multiple paths from source_dir" );
      FILEZ_STDERR( "  sharing the same inode will be created under sparse_dir is unspecified." );
      FILEZ_STDERR( "  Source and sparse dir must be on the same filesystem. Sparse dir must not exist." );
      return 1;
   }
   filez::link_first_work( paths.front(), paths.back() ).perform();
   return 0;
}
