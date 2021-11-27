# CMakeList.txt : CMake project for twmailer-server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_executable (twmailer-server "twmailer-server.cpp" "../../shared/socket.cpp" "../../shared/socket.h" "../../shared/validation.cpp" "../../shared/validation.h" "commands.cpp" "commands.h" "ldap.cpp" "ldap.h")

# TODO: Add tests and install targets if needed.
target_link_libraries("twmailer-server" Threads::Threads)
target_link_libraries("twmailer-server" uuid)
target_link_libraries("twmailer-server" ldap)