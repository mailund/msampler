find_package(Catch2 3 REQUIRED)

add_executable(test_initialisation test_initialisation.cpp)
target_link_libraries(test_initialisation PRIVATE msampler Catch2::Catch2WithMain)
add_test(Initialisation test_initialisation)

