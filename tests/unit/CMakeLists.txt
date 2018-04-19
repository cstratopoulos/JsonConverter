# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

#######################################
# Pull in Catch as an interface target
#######################################
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

####################################################
# Define a library of headers shared by unit tests
####################################################
add_library(converter_test_utils INTERFACE)

target_sources(converter_test_utils INTERFACE $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/boost_uuid.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/light.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/nested_ranges.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/optional_fields.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/person.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/point_3d.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/udt_ranges.hpp
>)

target_link_libraries(converter_test_utils
    INTERFACE ossiaco_converter
    INTERFACE Catch
)

add_subdirectory(core_final)
add_subdirectory(customization_points)