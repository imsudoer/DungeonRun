cmake_minimum_required(VERSION 3.20)
project(DungeonRun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/EHsc /W3 /utf-8)
endif()

add_executable(DungeonRun src/main.cpp)

if(WIN32)
    target_link_libraries(DungeonRun PRIVATE kernel32 user32)
endif()
