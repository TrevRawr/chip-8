cmake_minimum_required(VERSION 3.6)
project(chip_8)

# include custom cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(add_external_static_cmake_library)

# Set C++ standard
# make all warnings into compile time errors
# use a memory leak dynamic (runtime) sanitizer to detect memory leaks.
# Note: there seems to be memory leaks reported with some graphics and input drivers (ex: vmwgfx_dri - VMware's GL Driver, and libxi) that are out of this application's control
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -fsanitize=leak -fno-omit-frame-pointer -Werror -Wall -Wextra")

# Setup different source file variables
set(SOURCE_FILES src/cpu/Cpu.cpp src/cpu/Cpu.h src/subsystems/display/IDisplay.h src/subsystems/input/IInputController.h src/storage/Memory.cpp src/storage/Memory.h src/exceptions/IndexOutOfBoundsException.h src/constants/Constants.h src/exceptions/InstructionUnimplementedException.h src/exceptions/BaseException.h src/constants/OpcodeBitmasks.h src/constants/Opcodes.h src/exceptions/UnimplementedException.h src/constants/OpcodeBitshifts.h src/utils/RandomUtil.cpp src/utils/RandomUtil.h src/io/FileByteReader.cpp src/io/FileByteReader.h src/exceptions/IOException.h src/exceptions/InitializationException.h src/subsystems/ISubsystemManager.h src/Chip8.cpp src/Chip8.h src/utils/SleepUtil.cpp src/utils/SleepUtil.h)
# keep source files that are dependent on SDL library separate in order to exclude from testcases build (and possibly other builds in the future).
set(SDL_SOURCE_FILES src/subsystems/display/Display.cpp src/subsystems/display/Display.h src/subsystems/input/InputController.cpp src/subsystems/input/InputController.h src/subsystems/SdlSubsystemManager.cpp src/subsystems/SdlSubsystemManager.h src/main.cpp)
set(TESTING_SOURCE_FILES testcases/CpuTest.cpp testcases/CpuTestFixture.cpp testcases/CpuTestFixture.h ${SOURCE_FILES} testcases/main.cpp testcases/mocks/MockDisplay.h testcases/mocks/MockInputController.h)
set(ALL_SOURCE_FILES ${SOURCE_FILES} ${SDL_SOURCE_FILES} ${TESTING_SOURCE_FILES})

# makefile target to run clang-format on all built files
# See more at: https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format#sthash.nl8UE5nB.dpuf
add_custom_target(clangformat COMMAND cd .. && /usr/bin/clang-format -style=file -i ${ALL_SOURCE_FILES})

# Required for both GTest/GMock and SDL2 libraries
find_package(Threads REQUIRED)

# Setup SDL2 Library
add_external_static_cmake_library(libsdl2 https://www.libsdl.org/tmp/SDL-2.0.5-11113.zip libSDL2-2.0.dylib SHARED)
#add sdl dependency on threads library
set_target_properties(libsdl2 PROPERTIES
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Setup main emulator executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SDL_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${libsdl2_SRC}/include)
#the libraries after libsdl2 are dependencies of libsdl2. It won't compile without these
#thanks to https://github.com/SergNikitin/img_glypher/blob/master/CMakeLists.txt for helping me figure out how to resolve these dependency errors
target_link_libraries(${PROJECT_NAME} libsdl2)

#Allows CTest to be used (effectively enables the add_test() command)
enable_testing()

# Create a library target for testing with GTest
add_external_static_cmake_library(libgtest https://github.com/google/googletest/archive/master.zip googlemock/gtest/libgtest.a STATIC)
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a library target for testing with GMock
# GMock was downloaded with GTest, so we don't need to go through the whole process of adding an external static library
# Instead, we just point the new library target to the static library file for GMock
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock libgtest)
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${libgtest_BIN}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a testcases executable target
# add_test allows you to execute test cases using "make test" i.e. the test target in the generated Makefile.
add_executable(testcases ${TESTING_SOURCE_FILES})
target_include_directories(testcases PRIVATE "${libgtest_SRC}/googletest/include"
        "${libgtest_SRC}/googlemock/include")
target_link_libraries(testcases libgtest libgmock)
add_test(EmulatorTests testcases)