cmake_minimum_required(VERSION 2.8.2)

set(wget_src_list Downloader.cpp
                  ArgParser.cpp
                  toNetworkUris.cpp
                  )
set(wget_src_list_with_main main.cpp ${wget_src_list})
set_source_files_properties(${wget_src_list_with_main} PROPERTIES COMPILE_FLAGS "${COMMON_CXX_FLAGS}")

#################################################################
add_library(wget_src OBJECT ${wget_src_list})

#################################################################
add_executable(${CMAKE_PROJECT_NAME} main.cpp $<TARGET_OBJECTS:wget_src>)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} network-uri ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})

#################################################################
foreach(target wget_src ${CMAKE_PROJECT_NAME})
target_include_directories(${target} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/deps/Beast/include
                                                    ${CMAKE_SOURCE_DIR}/deps/uri/include
                                                    ${Boost_INCLUDE_DIRS})
endforeach(target)
