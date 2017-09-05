cmake_minimum_required(VERSION 3.8)

project(boundvariable)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -pedantic")
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UMVM_SOURCES
  src/umvm.cpp
  src/um.cpp)

set(DISUM_SOURCES
  src/um.cpp
  src/disum.cpp)

add_executable(umvm ${UMVM_SOURCES})
target_include_directories(umvm PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(disum ${DISUM_SOURCES})
target_include_directories(disum PRIVATE ${PROJECT_SOURCE_DIR}/include)
