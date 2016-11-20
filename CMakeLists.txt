cmake_minimum_required(VERSION 2.8.2)

project(wget C CXX) # C is necessary for find_package(Threads) for some reason

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Checking boost ...
find_package(Boost 1.58.0 REQUIRED COMPONENTS system program_options)
# ... and pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

###########################################################

include(Colors.cmake)

greenMessage("Handling vinniefalco/Beast")
add_subdirectory(deps/Beast EXCLUDE_FROM_ALL)

greenMessage("Handling cpp-netlib/uri")
set(Uri_FULL_WARNINGS OFF CACHE BOOL "")
set(Uri_WARNINGS_AS_ERRORS OFF CACHE BOOL "")
set(Uri_BUILD_TESTS OFF CACHE BOOL "")
set(Uri_BUILD_DOCS OFF CACHE BOOL "")
set(Uri_DISABLE_LIBCXX ON CACHE BOOL "")
add_subdirectory(deps/uri EXCLUDE_FROM_ALL)

###########################################################

# Checking compiler for C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
  message(FATAL_ERROR
          "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(wget_SRCS src/main.cpp
              src/Downloader.cpp
              )
add_executable(wget ${wget_SRCS})
set_source_files_properties(${wget_SRCS} PROPERTIES COMPILE_FLAGS
                            "-Wuninitialized -Wpedantic -Wextra -Wall -Wsign-conversion")
# target version doesn't propagate SYSTEM property
#target_include_directories(wget SYSTEM PRIVATE
include_directories(SYSTEM PRIVATE ${Boost_INCLUDE_DIRS} deps/Beast/include deps/uri/include)
target_link_libraries(wget network-uri ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
