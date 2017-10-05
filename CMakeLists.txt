cmake_minimum_required(VERSION 3.8)

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

if(DO_TRACE)
  add_definitions(-DTRACE)
endif()


include_directories(include)

add_library(dna OBJECT src/dna.cpp)
add_library(rna OBJECT src/rna.cpp)


# DNA -> execute -> RNA -> build -> IMAGE
add_executable(execute src/execute.cpp $<TARGET_OBJECTS:dna>)

add_executable(build src/build.cpp $<TARGET_OBJECTS:rna>)
target_link_libraries(build z png)


enable_testing()

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/t*-*.cpp)
foreach(test_src ${TEST_SRCS})
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src} $<TARGET_OBJECTS:dna> $<TARGET_OBJECTS:rna>)
  target_link_libraries(${test_name} z png)
  target_include_directories(${test_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
