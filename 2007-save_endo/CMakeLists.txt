cmake_minimum_required(VERSION 3.10)

project(save-endo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -Wno-c++98-compat -pedantic")
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_GLIBCXX_DEBUG)
endif()

set(DO_TRACE OFF CACHE BOOL "Enable trace output during execution.")
if(DO_TRACE)
  add_definitions(-DTRACE)
endif()

set(DO_SAVE_BITMAPS ON CACHE BOOL "Save bitmaps that might be lost by build.")
if(DO_SAVE_BITMAPS)
  add_definitions(-DSAVE_BITMAPS)
endif()

set(DO_COUNT_UNHANDLED_RNA ON CACHE BOOL "Count the number of unhandle RNA sequences.")
if (DO_COUNT_UNHANDLED_RNA)
  add_definitions(-DCOUNT_UNHANDLED_RNA)
endif()

set(DO_GPROF OFF CACHE BOOL "Enable gprof.")
if (DO_GPROF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()


# execute <dna>... -> RNA
add_executable(execute src/execute.cpp)

# build <rna> -> endo.png
add_executable(build src/build.cpp)
target_link_libraries(build z png)

# risk <prefix-dna> <source.png> <target.png>
add_executable(risk src/risk.cpp)
target_link_libraries(risk z png)

set(DO_UNIT_TESTING ON CACHE BOOL "Enable compiling unit tests.")
if (DO_UNIT_TESTING)
  enable_testing()

  file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/test-*.cpp)
  foreach(test_src ${TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} z png)
    target_include_directories(${test_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
