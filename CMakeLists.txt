cmake_minimum_required(VERSION 3.10)
project(pathfinder3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(pathfinder3d
        src/app/main.cpp
        src/model/Grid.cpp
        src/model/BFS.cpp
        src/view/Renderer.cpp
        src/view/Renderer.h
)

target_include_directories(pathfinder3d PRIVATE
        src
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
)

target_link_libraries(pathfinder3d
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
)
