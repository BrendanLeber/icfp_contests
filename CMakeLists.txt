cmake_minimum_required(VERSION 3.8)
include(ExternalProject)

project(lambda_punter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_XX_FLAGS} -Weverything -Wno-c++98-compat -Wno-padded -pedantic")
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None, Debug, Release, RelWithDebInfo and MinSizeRel."
    FORCE)
endif()


# nlohmann json
include_directories("${PROJECT_SOURCE_DIR}/third-party")


set(LAMBDA_PUNTER_SOURCES
  src/punter.cpp)

add_executable(punter ${LAMBDA_PUNTER_SOURCES})
