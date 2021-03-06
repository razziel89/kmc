cmake_minimum_required(VERSION 2.6)

project(votca-kmc)

set(PROJECT_VERSION "1.3-dev")
set(PROJECT_CONTACT "bugs@votca.org")

# Cmake modules/macros are in a subdirectory to keep this file cleaner
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)


enable_language(CXX)

######################################################################
# compiler tests
# these need ot be done early (before further tests).
#####################################################################

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(FATAL_ERROR "Could not find a C++-11 compiler")
endif()


########################################################################
# User input options                                                   #
########################################################################
option(BUILD_SHARED_LIBS "Build shared libs" ON)
if (NOT DEFINED LIB)
  set(LIB "lib")
endif(NOT DEFINED LIB)
if (NOT DEFINED MAN)
  set(MAN "share/man")
endif(NOT DEFINED MAN)
if (NOT DEFINED DATA)
  set(DATA "share/votca")
endif(NOT DEFINED DATA)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

########################################################################
#Find external packages
########################################################################
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.hg)
  find_package(Mercurial)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.hg)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  find_package(Git)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)

find_package(Boost 1.39.0 REQUIRED COMPONENTS program_options serialization )
include_directories(${Boost_INCLUDE_DIRS})
set (BOOST_LIBRARIES ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY} )
set (BOOST_CFLAGS_PKG "-I${Boost_INCLUDE_DIRS}")
string(REGEX REPLACE "^(.*)/lib([^/]*)\\.[^.]*$" "-L\\1 -l\\2" BOOST_LIBS_PKG ${BOOST_LIBRARIES})

option(BUILD_MANPAGES "Build manpages (might lead to problem on system without rpath" ON)
#define this target here, so that individual man pages can append to it.
if(NOT TARGET manpages)
  add_custom_target(manpages ALL)
endif()

find_package(VOTCA_TOOLS REQUIRED)
if(NOT VOTCA_TOOLS_HAS_SQLITE3)
  message(FATAL_ERROR "Votca tools was build without SQLITE3 support, rebuild it and try again")
endif(NOT VOTCA_TOOLS_HAS_SQLITE3)
include_directories(${VOTCA_TOOLS_INCLUDE_DIRS})

########################################################################
# Basic system tests (standard libraries, headers, functions, types)   #
########################################################################
include(CheckIncludeFile)
foreach(HEADER assert.h)
  check_include_file(${HEADER} FOUND_${HEADER})
  if(NOT FOUND_${HEADER})
    message(FATAL_ERROR "Could not find needed header - ${HEADER}")
  endif(NOT FOUND_${HEADER})
endforeach(HEADER)

include(CheckIncludeFileCXX)
foreach(HEADER algorithm)
  check_include_file_cxx(${HEADER} FOUND_${HEADER})
  if(NOT FOUND_${HEADER})
    message(FATAL_ERROR "Could not find needed header - ${HEADER}")
  endif(NOT FOUND_${HEADER})
endforeach(HEADER)

set(MATH_LIBRARIES "m" CACHE STRING "math library")
mark_as_advanced( MATH_LIBRARIES )
include(CheckLibraryExists)
foreach(FUNC sqrt)
  check_library_exists(${MATH_LIBRARIES} ${FUNC} "" FOUND_${FUNC}_${MATH_LIBRARIES})
  if(NOT FOUND_${FUNC}_${MATH_LIBRARIES})
    message(FATAL_ERROR "Could not find needed math function - ${FUNC}")
  endif(NOT FOUND_${FUNC}_${MATH_LIBRARIES})
endforeach(FUNC)

######################################
# Include the following subdirectory # 
######################################
add_subdirectory(include/votca/kmc)
add_subdirectory(src/libkmc)
add_subdirectory(src/tools)
add_subdirectory(share)
