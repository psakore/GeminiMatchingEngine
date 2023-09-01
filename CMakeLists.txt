cmake_minimum_required(VERSION 3.14)
project(MatchingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_FOR_TESTING OFF "Build for Testing")

if(BUILD_FOR_TESTING)
  enable_testing()
endif()

add_subdirectory(src)
