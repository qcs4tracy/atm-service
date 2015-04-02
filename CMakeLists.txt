cmake_minimum_required(VERSION 3.1)
project(cnp_atm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/)
include_directories(${CMAKE_SOURCE_DIR}/log)
#add_executable(cnp-atm main.cpp mysql_client.h mysql_client.c)
#target_link_libraries(cnp-atm mysqlclient)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/Document/clion")
#proto_impl.h proto_impl.cpp data_access.cpp data_access.h
#add_executable(cnp-atm-s socket.h socket.cpp main.cpp IOLoop.h IOLoop.cpp proto_impl.h proto_impl.cpp)
#target_link_libraries(cnp-atm-s ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
#target_link_libraries(cnp-atm-s pthread)
#
#add_executable(cnp-atm socket.h socket.cpp client.cpp)

add_executable(test_db data_access.cpp mysql_client.h mysql_client.c)
target_link_libraries(test_db mysqlclient)
target_link_libraries(test_db ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
target_link_libraries(test_db pthread)