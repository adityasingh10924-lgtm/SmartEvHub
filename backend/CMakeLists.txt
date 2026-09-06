cmake_minimum_required(VERSION 3.20)

project(SmartEVHub)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(evhub
    src/main.cpp
    src/Database.cpp
    src/sqlite3.c
)

target_include_directories(evhub PRIVATE
    include
)