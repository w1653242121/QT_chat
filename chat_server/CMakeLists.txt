cmake_minimum_required(VERSION 3.10)

project(main)

set(CMAKE_CXX_STANDRAD 11)

add_executable(main main.cpp chat_database.cpp chat_thread.cpp chat_list.cpp chat_server.cpp)

target_link_libraries(main event mysqlclient pthread jsoncpp)



















