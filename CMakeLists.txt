cmake_minimum_required(VERSION 3.25.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

project(
    msampler
    VERSION 1.0
    DESCRIPTION 
        "Constant time sampling from a multinomial after linear time preprocessing"
    LANGUAGES CXX
)

# Set these only if this is the main CMake file we are not
# included from somewhere else...
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS OFF)
    
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("Enabling code coverage tracking")
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage_lcov(
            NAME coverage                 
            EXECUTABLE ctest -j ${n_cores} # Executable in PROJECT_BINARY_DIR
        )
    endif()
    enable_testing()
    
    add_subdirectory(test)


    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

add_subdirectory(src)
