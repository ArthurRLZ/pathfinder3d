cmake_minimum_required(VERSION 4.1)
project(pathfinder3d)

set(CMAKE_CXX_STANDARD 20)

add_executable(pathfinder3d src/app/main.cpp
        src/model/Grid.cpp
        src/model/Grid.h
        src/model/BFS.cpp
        src/model/BFS.h
        src/model/Cell.h
        src/model/CellType.h)

target_include_directories(pathfinder3d PRIVATE src)