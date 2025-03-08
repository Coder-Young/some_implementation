cmake_minimum_required(VERSION 3.10)

project(myConnectionPool VERSION 1.0)

aux_source_directory(./src SRC_LIST)

include_directories(./include)

add_executable(myConnectionPool ${SRC_LIST})

target_link_libraries(myConnectionPool PRIVATE pthread)
target_link_libraries(myConnectionPool PRIVATE mysqlclient)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")