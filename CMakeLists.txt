cmake_minimum_required(VERSION 3.6)
project(chip_8)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp)
add_executable(chip_8 ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif(NOT OPENGL_FOUND)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found!")
endif(NOT GLEW_FOUND)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARY})

#Not sure why this doesn't work...
#find_package(SDL2 REQUIRED)
#if(NOT SDL2_FOUND)
#    message(FATAL_ERROR "SDL2 not found!")
#endif(NOT SDL2_FOUND)
#include_directories(${SDL2_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} )
