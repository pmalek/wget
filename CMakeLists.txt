cmake_minimum_required(VERSION 2.8.2)

project(wget C CXX) # C is necessary for find_package(Threads) for some reason

# Checking compiler for C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
  message(FATAL_ERROR
          "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options(-Wuninitialized -Wpedantic -Wextra -Wall -Wsign-conversion)

add_executable(wget
  src/main.cpp
  src/Downloader.cpp
  )

# Checking boost ...
find_package(Boost 1.58.0 REQUIRED COMPONENTS system program_options)
target_link_libraries(wget ${Boost_LIBRARIES})
target_include_directories(wget SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

# ... and pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
target_link_libraries(wget ${CMAKE_THREAD_LIBS_INIT})

###########################################################

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
include(Colors.cmake)
greenMessage("Handling vinniefalco/Beast")
download_project(PROJ                Beast
                 GIT_REPOSITORY      https://github.com/vinniefalco/Beast.git
                 GIT_TAG             62e65ed
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
add_subdirectory(${Beast_SOURCE_DIR} ${Beast_BINARY_DIR} EXCLUDE_FROM_ALL)
target_include_directories(wget SYSTEM PRIVATE ${Beast_SOURCE_DIR}/include)
