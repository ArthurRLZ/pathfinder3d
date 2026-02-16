cmake_minimum_required(VERSION 3.10)
project(pathfinder3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenGL REQUIRED )
find_package(GLUT REQUIRED )

include_directories(#${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
        include_directories(src)

        add_executable(pathfinder3d
        src/app/main.cpp
        src/model/Grid.cpp
        src/model/Grid.h
        src/model/BFS.cpp
        src/model/BFS.h
        src/model/Cell.h
        src/model/CellType.h
)

        target_link_libraries(pathfinder3d ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

)