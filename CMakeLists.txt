cmake_minimum_required(VERSION 3.6)
project(chip_8)

#Set C++ standard, and make all warnings into compile time errors
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

#Setup main emulator executable
set(SOURCE_FILES src/cpu/Cpu.cpp src/cpu/Cpu.h src/display/Display.cpp src/display/Display.h src/display/IDisplay.h src/Chip8.cpp src/Chip8.h src/input/IInputController.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h src/input/InputController.cpp src/input/InputController.h)
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

#Setup SDL2 Library dependencies
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})

#Setup testing with GTest and GMock libraries
#big thanks to http://www.kaizou.org/2014/11/gtest-cmake/ for helping me get this set up
enable_testing()
# We need thread support for GTest
find_package(Threads REQUIRED)
include(ExternalProject)
# Download and install GoogleTest
ExternalProject_Add(
        gtest
        URL https://github.com/google/googletest/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
)
# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)
# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
# Set libgtest properties
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )
# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )
# Set include directories
#TODO: try using INTERFACE_INCLUDE_DIRECTORIES?
include_directories("${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")

#TODO: remove dependency on SDL2_LIBRARIES from testcases
add_executable(testcases testcases/CpuTest.cpp testcases/CpuTestFixture.cpp testcases/CpuTestFixture.h ${SOURCE_FILES} testcases/main.cpp testcases/mocks/MockDisplay.h testcases/mocks/MockInputController.h)
target_link_libraries(testcases libgtest libgmock ${SDL2_LIBRARIES})
add_test(AllTestsInFoo testcases)
