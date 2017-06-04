cmake_minimum_required(VERSION 3.6)
project(chip_8)

#PThread required by google test
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pthread")

set(SOURCE_FILES src/cpu/Cpu.cpp src/cpu/Cpu.h src/display/Display.h src/Chip8.cpp src/Chip8.h src/input/InputController.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/display/Display.cpp src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h src/input/InputController.cpp)
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(testcases testcases/CpuTest.cpp testcases/CpuTestFixture.cpp testcases/CpuTestFixture.h ${SOURCE_FILES})
target_link_libraries(testcases ${GTEST_BOTH_LIBRARIES} ${SDL2_LIBRARIES})
add_test(AllTestsInFoo testcases)

