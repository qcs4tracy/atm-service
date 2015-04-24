cmake_minimum_required(VERSION 2.8)
project(cnp_atm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/)
include_directories(${CMAKE_SOURCE_DIR}/log)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/Document/clion")
add_executable(cnp-atm-server socket.h socket.cpp main.cpp IOLoop.h IOLoop.cpp proto_impl.h proto_impl.cpp ClientsManager.cpp ClientsManager.h AtmDataAccess.cpp AtmDataAccess.h)
target_link_libraries(cnp-atm-server ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
target_link_libraries(cnp-atm-server pthread)
target_link_libraries(cnp-atm-server mysqlcppconn)

add_executable(cnp-atm-client socket.h socket.cpp client.cpp time_utils.h time_utils.cpp)
target_link_libraries(cnp-atm-client ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
target_link_libraries(cnp-atm-client pthread)
