cmake_minimum_required(VERSION 3.25.1)

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
