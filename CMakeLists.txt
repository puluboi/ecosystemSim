
cmake_minimum_required(VERSION 3.10)
project(CPP-PROJECT)
set(CMAKE_CXX_STANDARD 17)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
add_executable(exe 
src/main.cpp 
src/game.cpp
src/entity.cpp
src/obstacle.cpp
src/nutrient.cpp
src/graphics/Viz.cpp)
target_link_libraries(exe sfml-graphics sfml-window sfml-system)