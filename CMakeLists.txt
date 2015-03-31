cmake_minimum_required(VERSION 3.1)
project(cnp_atm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/)
#add_executable(cnp-atm main.cpp mysql_client.h mysql_client.c)
#target_link_libraries(cnp-atm mysqlclient)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/Document/clion")
add_executable(cnp-atm-s socket.h socket.cpp main.cpp IOLoop.h IOLoop.cpp)
add_executable(cnp-atm socket.h socket.cpp client.cpp)