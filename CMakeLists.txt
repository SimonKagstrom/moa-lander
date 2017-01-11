cmake_minimum_required (VERSION 2.8)
project (moa-lander)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)

add_custom_command(
   OUTPUT lander.bmp
   COMMAND convert ${PROJECT_SOURCE_DIR}/lander.xcf lander.bmp
)
add_custom_command(
   OUTPUT sparks.bmp
   COMMAND convert ${PROJECT_SOURCE_DIR}/sparks.xcf sparks.bmp
)
add_custom_command(
   OUTPUT person.bmp
   COMMAND convert ${PROJECT_SOURCE_DIR}/person.xcf person.bmp
)
add_custom_command(
   OUTPUT lander-full.bmp
   COMMAND convert ${PROJECT_SOURCE_DIR}/lander-full.xcf lander-full.bmp
)


include_directories(
	${SDL2_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/src/include
)

set (CMAKE_CXX_FLAGS "-g -O0 --std=c++1y")

add_executable(moa-lander
	src/main.cc
	src/game.cc
)

set_property(SOURCE src/game.cc APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/lander.bmp)
set_property(SOURCE src/game.cc APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/sparks.bmp)
set_property(SOURCE src/game.cc APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/person.bmp)
set_property(SOURCE src/game.cc APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/lander-full.bmp)

target_link_libraries(moa-lander ${SDL2_LIBRARY})

