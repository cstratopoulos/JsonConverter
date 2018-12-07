# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

####################################################
# Define a library of headers shared by unit tests
####################################################
add_library(converter_test_utils
    ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/chess.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/digraph.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/boost_uuid.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/light.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/messages.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/messages2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/nested_ranges.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/optional_fields.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/person.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/point_3d.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/shape.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/sprint_record.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/udt_ranges.hpp
)

target_link_libraries(converter_test_utils
    PUBLIC Catch2::Catch2
    PUBLIC ossiaco_converter
)

if(${OSSIACO_CONVERTER_BOOST_FS})
    target_link_libraries(converter_test_utils
        PUBLIC Boost::filesystem
    )
endif()

add_subdirectory(core_final)
add_subdirectory(customization_points)
add_subdirectory(enum_mapped)
add_subdirectory(poly_decorator)
