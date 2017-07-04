# Big thanks to http://www.kaizou.org/2014/11/gtest-cmake/ for initially helping me use ExternalProject to setup libraries
# Though, since reading this link, I have heavily modified the provided example to be more generic as demonstrated in the function below

# module required for including external projects (libraries)
include(ExternalProject)

# Configures an external CMake project and creates a static library target for it
# <url> should point to an archive of a CMake library with a CMakeLists.txt in the root directory
# An external project will be created and populated by the contents at the given <url>
# <static_library_path> should be a file path (or name) relative to the external project's binary directory pointing to the external project's built static library file
# A new library target will be created with the name <libname>
# Additionally, two variables will be initialized as "return values": <libname>_SRC and <libname>_BIN that point to
# the source directory and the binary directory of the external project respectively
function(add_external_static_cmake_library libname url static_library_path libtype)
    set(EXTERNAL_PROJECT_NAME ${libname}_EXTERNAL_PROJECT)
    # Setup an external project for the library, and download it. put it in its own folder.
    ExternalProject_Add(
            ${EXTERNAL_PROJECT_NAME}
            URL ${url}
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${libname}
            INSTALL_COMMAND ""
    )

    # Get source and binary directories from external CMake project
    ExternalProject_Get_Property(${EXTERNAL_PROJECT_NAME} SOURCE_DIR BINARY_DIR)

    # SRC is set in the parent, and can be thought of kind of like a return value of this function
    # BIN is set in the function scope and the parent scope, and is "returned" as well as used locally
    set(${libname}_SRC ${SOURCE_DIR} PARENT_SCOPE)
    set(${libname}_BIN ${BINARY_DIR} PARENT_SCOPE)
    set(${libname}_BIN ${BINARY_DIR})

    # Setup a library target (in this project) containing the built static library file from the external project.
    # The key part here is setting IMPORTED_LOCATION to the location of the built external project's static library file.
add_library(${libname} IMPORTED ${libtype} GLOBAL)
    set_target_properties(${libname} PROPERTIES
            "IMPORTED_LOCATION" "${${libname}_BIN}/${static_library_path}"
            )
    add_dependencies(${libname} ${EXTERNAL_PROJECT_NAME})
endfunction()