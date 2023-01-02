// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define _DARWIN_USE_64_BIT_INODE 64

#define FILEZ_ASSERT( eXPReSSioN )              \
   assert( eXPReSSioN )

#define FILEZ_STDOUT( MeSSaGe )                         \
   do { std::cout << MeSSaGe << std::endl; } while( 0 )

#define FILEZ_STDERR( MeSSaGe )                         \
   do { std::cerr << MeSSaGe << std::endl; } while( 0 )

#define FILEZ_ERROR( MeSSaGe )                                          \
   do { std::ostringstream oss; oss << MeSSaGe; throw std::runtime_error( oss.str() ); } while( 1 )

#define FILEZ_ERRNO( MeSSaGe )                                          \
   do { std::ostringstream oss; oss << MeSSaGe << " errno [ " << std::strerror( errno ) << " ]"; throw std::runtime_error( oss.str() ); } while( 1 )
