cmake_minimum_required (VERSION 2.8)
project (moa-lander)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set (CMAKE_CXX_FLAGS "-g -O0 --std=c++1y")

add_executable(moa-lander
	src/main.cc
)

target_link_libraries(moa-lander ${SDL2_LIBRARY})

