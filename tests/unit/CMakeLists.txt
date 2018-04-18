set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(converter_tests 
    test_main.cpp
    test_utils.hpp
    final_primitive.cpp
)

target_link_libraries(converter_tests 
    PUBLIC ossiaco_converter
    PUBLIC Catch
)
