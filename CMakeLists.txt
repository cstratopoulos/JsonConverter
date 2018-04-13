# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

cmake_minimum_required(VERSION 3.9)

project(OssiacoConverter
    VERSION 0.1.0
    LANGUAGES CXX
)

#######################
# Configuration macros
#######################

option(OSSIACO_WCHAR_UNICODE "whether to enable wchar_t as the primary character/string element type for using unicode on windows" OFF)
if(${OSSIACO_WCHAR_UNICODE})
    set(_ossiaco_wchar_unicode 1)
else()
    set(_ossiaco_wchar_unicode 0)
endif()

option(OSSIACO_RAPIDJSON_ASSERT_THROW "whether to redefine RAPIDJSON_ASSERT to throw exceptions on assertion failure" OFF)
if(${OSSIACO_RAPIDJSON_ASSERT_THROW})
    set(_ossiaco_rapidjson_assert_throw 1)
else()
    set(_ossiaco_rapidjson_assert_throw 0)
endif()

####################################
# Library dependency paths/packages
####################################

find_path(RAPIDJSON_INCLUDE_DIR rapidjson/rapidjson.h)

find_package(Boost 1.66.0 REQUIRED)

###############################
# The converter library target
###############################

add_library(ossiaco_converter INTERFACE)

target_sources(ossiaco_converter INTERFACE $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/template_get_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/char_types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/type_tree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/hooks/logging.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/is_detected.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/detect_specialization.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/file_handle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/print_type_name.hpp
>)

target_include_directories(ossiaco_converter INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_include_directories(ossiaco_converter
    INTERFACE ${RAPIDJSON_INCLUDE_DIR}
    INTERFACE ${Boost_INCUDE_DIRS}
)

target_compile_features(ossiaco_converter INTERFACE cxx_std_17)

target_compile_definitions(ossiaco_converter INTERFACE
    OSSIACO_WCHAR_UNICODE=${_ossiaco_wchar_unicode}
    OSSIACO_RAPIDJSON_ASSERT_THROW=${_ossiaco_rapidjson_assert_throw}
)

if(MSVC)
    target_compile_definitions(ossiaco_converter INTERFACE 
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    )
endif()

#################
# The unit tests
#################

add_subdirectory(tests)
