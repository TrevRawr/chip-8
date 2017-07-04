cmake_minimum_required(VERSION 3.6)
project(chip_8)

#Set C++ standard
#make all warnings into compile time errors
#use a memory leak dynamic (runtime) sanitizer to detect memory leaks.
#Note: there seems to be memory leaks reported with some graphics and input drivers (ex: vmwgfx_dri - VMware's GL Driver, and libxi) that are out of this application's control
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=leak -fno-omit-frame-pointer -Werror -Wall -Wextra")

#Setup different source file variables
set(SOURCE_FILES src/cpu/Cpu.cpp src/cpu/Cpu.h src/subsystems/display/IDisplay.h src/subsystems/input/IInputController.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h src/exceptions/InitializationException.h src/subsystems/ISubsystemManager.h src/Chip8.cpp src/Chip8.h src/utils/SleepUtil.cpp src/utils/SleepUtil.h)
#keep source files that are dependent on SDL library separate in order to exclude from testcases build (and possibly other builds in the future).
set(SDL_SOURCE_FILES src/subsystems/display/Display.cpp src/subsystems/display/Display.h src/subsystems/input/InputController.cpp src/subsystems/input/InputController.h src/subsystems/SdlSubsystemManager.cpp src/subsystems/SdlSubsystemManager.h src/main.cpp)
set(TESTING_SOURCE_FILES testcases/CpuTest.cpp testcases/CpuTestFixture.cpp testcases/CpuTestFixture.h ${SOURCE_FILES} testcases/main.cpp testcases/mocks/MockDisplay.h testcases/mocks/MockInputController.h)
set(ALL_SOURCE_FILES ${SOURCE_FILES} ${SDL_SOURCE_FILES} ${TESTING_SOURCE_FILES})

#Setup main emulator executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SDL_SOURCE_FILES})

#makefile target to run clang-format on all built files
#See more at: https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format#sthash.nl8UE5nB.dpuf
add_custom_target(clangformat COMMAND cd .. && /usr/bin/clang-format -style=file -i ${ALL_SOURCE_FILES})

#Required for both GTest and SDL2
find_package(Threads REQUIRED)

#module required for including external projects (libraries)
include(ExternalProject)

#Setup SDL2 Library Dependencies
ExternalProject_Add(
        sdl2
        URL https://www.libsdl.org/tmp/SDL-2.0.5-11113.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/sdl2
        INSTALL_COMMAND ""
)
ExternalProject_Get_Property(sdl2 SOURCE_DIR)
ExternalProject_Get_Property(sdl2 BINARY_DIR)
set(SDL2_SRC ${SOURCE_DIR})
set(SDL2_BIN ${BINARY_DIR})
add_library(libsdl2 IMPORTED STATIC GLOBAL)
set_target_properties(libsdl2 PROPERTIES
        "IMPORTED_LOCATION" "${SDL2_BIN}/libSDL2.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_SRC}/include)
add_dependencies(libsdl2 sdl2)
#the libraries after libsdl2 are dependencies of libsdl2. It won't compile without these
#thanks to https://github.com/SergNikitin/img_glypher/blob/master/CMakeLists.txt for helping me figure out how to resolve these errors
target_link_libraries(${PROJECT_NAME} libsdl2 m dl sndio)


#Setup testing with GTest and GMock libraries
#big thanks to http://www.kaizou.org/2014/11/gtest-cmake/ for helping me get this set up
enable_testing()
# We need thread support for GTest
# Download and install GoogleTest
ExternalProject_Add(
        gtest
        URL https://github.com/google/googletest/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
)
# Get GTest source and binary directories from CMake project
externalProject_get_property(gtest SOURCE_DIR BINARY_DIR)
set(GTEST_SRC ${SOURCE_DIR})
set(GTEST_BIN ${BINARY_DIR})
# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
# Set libgtest properties. libraries included from already built external projects need to specify their location with the below properties.
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${GTEST_BIN}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )
# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${GTEST_BIN}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )


#add_test allows you to execute test cases using "make test" i.e. the test target in the generated Makefile.
#running this target only works from inside the build folder (Otherwise, there's no Makefile!)
add_executable(testcases ${TESTING_SOURCE_FILES})
# Set include directories
target_include_directories(testcases PRIVATE "${GTEST_SRC}/googletest/include"
        "${GTEST_SRC}/googlemock/include")
target_link_libraries(testcases libgtest libgmock)
add_test(EmulatorTests testcases)
