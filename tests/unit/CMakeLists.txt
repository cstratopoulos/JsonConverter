# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(converter_tests 
    test_main.cpp
    test_utils.hpp
    types/light.hpp
    types/person.hpp
    types/point_3d.hpp
    final_primitive.cpp
)

target_link_libraries(converter_tests 
    PUBLIC ossiaco_converter
    PUBLIC Catch
)
