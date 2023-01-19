set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(
    docs 
    msampler.hpp
    "${CMAKE_CURRENT_SOURCE_DIR}/docs.md"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
)
