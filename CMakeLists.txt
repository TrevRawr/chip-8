cmake_minimum_required(VERSION 3.6)
project(chip_8)

#Set C++ standard
#make all warnings into compile time errors
#use a memory leak dynamic (runtime) sanitizer to detect memory leaks.
#Note: there seems to be memory leaks reported with some graphics and input drivers (ex: vmwgfx_dri - VMware's GL Driver, and libxi) that are out of this application's control
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=leak -fno-omit-frame-pointer -Werror -Wall -Wextra")

#Setup main emulator executable
set(SOURCE_FILES src/cpu/Cpu.cpp src/cpu/Cpu.h src/subsystems/display/IDisplay.h src/subsystems/input/IInputController.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h src/exceptions/InitializationException.h src/subsystems/ISubsystemManager.h src/Chip8.cpp src/Chip8.h src/utils/SleepUtil.cpp src/utils/SleepUtil.h)
#keep source files that are dependent on SDL library separate in order to exclude from testcases build (and possibly other builds in the future).
set(SDL_SOURCE_FILES src/subsystems/display/Display.cpp src/subsystems/display/Display.h src/subsystems/input/InputController.cpp src/subsystems/input/InputController.h src/subsystems/SdlSubsystemManager.cpp src/subsystems/SdlSubsystemManager.h src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SDL_SOURCE_FILES})

#Setup SDL2 Library dependencies
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

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
externalProject_get_property(gtest source_dir binary_dir)
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
include_directories("${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")

#add_test allows you to execute test cases using "make test" i.e. the test target in the generated Makefile.
#running this target only works from inside the build folder (Otherwise, there's no Makefile!)
set(TESTING_SOURCE_FILES testcases/CpuTest.cpp testcases/CpuTestFixture.cpp testcases/CpuTestFixture.h ${SOURCE_FILES} testcases/main.cpp testcases/mocks/MockDisplay.h testcases/mocks/MockInputController.h)
add_executable(testcases ${TESTING_SOURCE_FILES})
target_link_libraries(testcases libgtest libgmock)
add_test(EmulatorTests testcases)
