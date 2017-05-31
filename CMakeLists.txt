cmake_minimum_required(VERSION 3.6)
project(chip_8)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(SOURCE_FILES src/main.cpp src/cpu/Cpu.cpp src/cpu/Cpu.h src/display/Display.h src/Chip8.cpp src/Chip8.h src/input/Input.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/display/Display.cpp src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h)
add_executable(chip_8 ${SOURCE_FILES})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})
