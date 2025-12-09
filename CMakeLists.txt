cmake_minimum_required(VERSION 3.10)
project(mysql_example)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)

# Zmieniaj ścieżki do Connector/C++ jeśli trzeba
include_directories(/opt/mysql-connector/include)
link_directories(/opt/mysql-connector/lib64)

add_executable(kod kod.cpp)
target_link_libraries(kod yaml-cpp mysqlcppconn mysqlcppconnx)
