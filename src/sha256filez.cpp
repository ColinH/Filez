// Copyright (c) 2023-2025 Dr. Colin Hirsch - All Rights Reserved

#include <filesystem>
#include <iostream>

#include "data_hash.hpp"
#include "file_mmap.hpp"

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      const std::filesystem::path path( argv[ i ] );
      const filez::file_mmap mmap( path );
      std::cout << filez::data_hash( mmap.data(), mmap.size() ).result() << " " << path << std::endl;
   }
   return 0;
}
