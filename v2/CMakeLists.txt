cmake_minimum_required(VERSION 3.10)
project(Mazeology)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

add_executable(Mazeology main.cpp lodepng.cpp
        constants.h
        cell.h
        maze_utils.h
        image_utils.h
        maze_utils.cpp
        image_utils.cpp
        image_utils.cpp
        lodepng.cpp
)

target_link_libraries(Mazeology sfml-graphics sfml-window sfml-system)
