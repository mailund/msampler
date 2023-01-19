add_library(msampler 
    msampler.cpp
)
target_compile_features(msampler 
    PUBLIC cxx_std_20
)
target_include_directories(msampler
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# FIXME: install code...
