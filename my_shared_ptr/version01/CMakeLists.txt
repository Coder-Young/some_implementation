cmake_minimum_required(VERSION 2.8)

project(my_shared_ptr)

include_directories(./include)

aux_source_directory(./src SRC_LIST)

add_executable(my_shared_ptr ${SRC_LIST})

target_link_libraries(my_shared_ptr PRIVATE pthread)