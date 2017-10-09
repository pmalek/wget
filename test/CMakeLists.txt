#set(GMOCK_ROOT ${CMAKE_SOURCE_DIR}/deps/googletest/googlemock/)
#set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/deps/googletest/googletest/)
#find_package(GTest REQUIRED)

#add_subdirectory(${CMAKE_SOURCE_DIR}/deps/googletest/googletest/ gtest/)
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/googletest/googlemock/ gmock/)
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/rapidcheck/ rapidcheck/)

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)

        get_filename_component(testName ${testSrc} NAME_WE)

        message(STATUS )
        message(STATUS ${testName})
        message(STATUS )

        #Add compile target
        add_executable(${testName} $<TARGET_OBJECTS:wget_src> ${testSrc})

        target_include_directories(${testName} PRIVATE ${CMAKE_SOURCE_DIR}/src/
                                                       ${CMAKE_SOURCE_DIR}/deps/Beast/include/
                                                       ${CMAKE_SOURCE_DIR}/deps/googletest/googletest/include/
                                                       ${CMAKE_SOURCE_DIR}/deps/rapidcheck/extras/gtest/include/
                                                       ${CMAKE_SOURCE_DIR}/deps/uri/include)

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} gmock_main rapidcheck network-uri ${Boost_LIBRARIES} pthread)

        #I like to move testing binaries into a test directory
        set_target_properties(${testName} PROPERTIES
                              RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
                 COMMAND ${testName})
endforeach(testSrc)

