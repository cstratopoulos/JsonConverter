# Ossiaco JSON Converter
#
# Copyright 2018 Ossiaco
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions: 
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software. 
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  

cmake_minimum_required(VERSION 3.9)

project(OssiacoConverter
    VERSION 0.1.0
    LANGUAGES CXX
)

#######################
# Configuration macros
#######################

option(OSSIACO_UNICODE "whether to use whcar_t as the primary character type and string element type" OFF)
if(${OSSIACO_UNICODE})
    set(_ossiaco_unicode 1)
else()
    set(_ossiaco_unicode 0)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/hooks/logging.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/is_detected.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/detect_specialization.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/print_type_name.hpp
>)

target_include_directories(ossiaco_converter INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_include_directories(ossiaco_converter
    INTERFACE ${RAPIDJSON_INCLUDE_DIR}
    INTERFACE ${Boost_INCUDE_DIRS}
)

target_compile_features(ossiaco_converter INTERFACE cxx_std_17)

target_compile_definitions(ossiaco_converter INTERFACE
    OSSIACO_UNICODE=${_ossiaco_unicode}
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
