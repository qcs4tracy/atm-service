cmake_minimum_required(VERSION 3.1)
project(cnp_atm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/)
include_directories(${CMAKE_SOURCE_DIR}/log)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/Document/clion")
add_executable(cnp-atm-s socket.h socket.cpp main.cpp IOLoop.h IOLoop.cpp proto_impl.h proto_impl.cpp ClientsManager.cpp ClientsManager.h AtmDataAccess.cpp AtmDataAccess.h)
target_link_libraries(cnp-atm-s ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
target_link_libraries(cnp-atm-s pthread)
target_link_libraries(cnp-atm-s mysqlcppconn)

add_executable(cnp-atm socket.h socket.cpp client.cpp time_utils.h time_utils.cpp)
target_link_libraries(cnp-atm ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
target_link_libraries(cnp-atm pthread)

#add_executable(test_db AtmDataAccess.cpp AtmDataAccess.h time_utils.cpp time_utils.h ClientsManager.cpp ClientsManager.h)
#target_link_libraries(test_db mysqlcppconn)
#target_link_libraries(test_db ${CMAKE_SOURCE_DIR}/log/liblog4z.a)
#target_link_libraries(test_db pthread)