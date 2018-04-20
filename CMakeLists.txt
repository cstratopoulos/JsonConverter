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

option(OSSIACO_CONVERTER_TESTS "whether to build the unit/integration tests" OFF)

option(OSSIACO_RAPIDJSON_ASSERT_THROW "whether to redefine RAPIDJSON_ASSERT to throw exceptions on assertion failure" OFF)
if(${OSSIACO_RAPIDJSON_ASSERT_THROW})
    set(_ossiaco_rapidjson_assert_throw 1)
else()
    set(_ossiaco_rapidjson_assert_throw 0)
endif()

option(OSSIACO_WCHAR_UNICODE "whether to enable wchar_t as the primary character/string element type for using unicode on windows" OFF)
if(${OSSIACO_WCHAR_UNICODE})
    set(_ossiaco_wchar_unicode 1)
else()
    set(_ossiaco_wchar_unicode 0)
endif()

####################################
# Library dependency paths/packages
####################################

find_path(DATE_INCLUDE_DIR date/date.h)
find_path(RAPIDJSON_INCLUDE_DIR rapidjson/rapidjson.h)
find_path(RANGE_V3_INCLUDE_DIR range/v3/range.hpp)

find_package(Boost 1.67.0 REQUIRED)

###############################
# The converter library target
###############################

add_library(ossiaco_converter INTERFACE)

target_sources(ossiaco_converter INTERFACE $<BUILD_INTERFACE:
    # Top level or bulk-include files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/adapt.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/converter.hpp

    # Deserialization allocator backends
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/decorator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/dispatch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/enum_to_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/mapped.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/simple.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/allocate/type_allocator.hpp

    # Converter components
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/chrono_fmt_prop_converter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/custom_value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/dispatch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/first_class.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/property_converter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/ranges.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/smart_ptr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/template_get_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/vocab_type.hpp

    # Core definitions/classes
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/buffers_streams_writers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/char_types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/not_found.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/pointer_wrapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/reference_mapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/type_tree.hpp

    # Customization point hooks
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/hooks/logging.hpp

    # Serialization/deserialization
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/serialize/deserializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/serialize/json_converter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/serialize/to_json_pretty.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/serialize/serializer.hpp

    # Miscellaneous utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/customized.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/detect_specialization.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/file_handle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/inconstructible.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/print_type_name.hpp
>)

target_include_directories(ossiaco_converter INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_include_directories(ossiaco_converter
    INTERFACE ${Boost_INCUDE_DIRS}
    INTERFACE ${DATE_INCLUDE_DIR}
    INTERFACE ${RAPIDJSON_INCLUDE_DIR}
)

target_compile_features(ossiaco_converter INTERFACE cxx_std_17)

target_compile_definitions(ossiaco_converter INTERFACE
    OSSIACO_RAPIDJSON_ASSERT_THROW=${_ossiaco_rapidjson_assert_throw}
    OSSIACO_WCHAR_UNICODE=${_ossiaco_wchar_unicode}
)

target_compile_definitions(ossiaco_converter INTERFACE
    RAPIDJSON_HAS_STDSTRING
)

if(MSVC)
    target_compile_definitions(ossiaco_converter INTERFACE 
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
        _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING
    )
endif()

#################
# The unit tests
#################

if(${OSSIACO_CONVERTER_TESTS})
    enable_testing()
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif()
