cmake_minimum_required(VERSION 2.8.2)

project(wget C CXX) # C is necessary for find_package(Threads) for some reason

option(WGET_BUILD_TESTS "Build wget's unit tests" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(OpenSSL REQUIRED)

# Checking boost ...
find_package(Boost 1.58.0 REQUIRED COMPONENTS system program_options)
# ... and pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(OpenSSL REQUIRED)

###########################################################

include(Colors.cmake)

greenMessage("Handling vinniefalco/Beast...")
add_subdirectory(deps/Beast EXCLUDE_FROM_ALL)

greenMessage("Handling cpp-netlib/uri...")
set(Uri_FULL_WARNINGS OFF CACHE BOOL "")
set(Uri_WARNINGS_AS_ERRORS OFF CACHE BOOL "")
set(Uri_BUILD_TESTS OFF CACHE BOOL "")
set(Uri_BUILD_DOCS OFF CACHE BOOL "")
set(Uri_DISABLE_LIBCXX ON CACHE BOOL "")
add_subdirectory(deps/uri EXCLUDE_FROM_ALL)

###########################################################

greenMessage("wget...")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(COMMON_CXX_FLAGS "-Wuninitialized -Wpedantic -Wextra -Wall -Wsign-conversion")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0)
message("Compiling with gcc...")
set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -Werror=suggest-override")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
message("Compiling with clang...")
endif()

###########################################################
# copmilation databse for YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
IF( EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json" )
  EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  )
ENDIF()
###########################################################

add_subdirectory(src/)

###########################################################
# tests
if(WGET_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(WGET_BUILD_TESTS)
