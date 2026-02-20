cmake_minimum_required(VERSION 3.10)
project(pathfinder3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use GLVND for modern OpenGL (Linux)
if(UNIX)
    cmake_policy(SET CMP0072 NEW)
endif()

# Add executable
add_executable(pathfinder3d
        src/app/main.cpp
        src/model/Grid.cpp
        src/model/BFS.cpp
        src/view/Renderer.cpp
        src/view/Renderer.h
)

# Platform-specific linking
if(WIN32)
    # Windows: assuming GLUT headers/libs are in PATH or installed via vcpkg/MSYS2
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    target_include_directories(pathfinder3d PRIVATE
            src
            ${OPENGL_INCLUDE_DIR}
            ${GLUT_INCLUDE_DIRS}
    )

    target_link_libraries(pathfinder3d
            ${GLUT_LIBRARY}
            ${OPENGL_LIBRARIES}
    )
else() # Linux / UNIX
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    target_include_directories(pathfinder3d PRIVATE
            src
            ${OPENGL_INCLUDE_DIRS}
            ${GLUT_INCLUDE_DIRS}
    )

    target_link_libraries(pathfinder3d
            ${GLUT_LIBRARIES}
            ${OPENGL_LIBRARIES}
    )
endif()