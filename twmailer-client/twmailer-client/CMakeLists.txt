# CMakeList.txt : CMake project for twmailer-client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (twmailer-client "twmailer-client.cpp" "../../shared/socket.cpp" "../../shared/socket.h" "commands.cpp" "commands.h")

# TODO: Add tests and install targets if needed.