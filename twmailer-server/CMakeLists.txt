# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("twmailer-server")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -luuid -pthread -lldap -llber")

find_package(Threads REQUIRED)
# Include sub-projects.
add_subdirectory ("twmailer-server")
