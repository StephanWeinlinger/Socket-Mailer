# CMakeList.txt : CMake project for twmailer-server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (twmailer-server "twmailer-server.cpp"  )

# TODO: Add tests and install targets if needed.

set_property(TARGET "twmailer-server" PROPERTY CXX_STANDARD 17)