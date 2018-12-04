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

option(OSSIACO_CONVERTER_TESTS "Build the unit/integration tests" OFF)
option(OSSIACO_CONVERTER_EXAMPLES "Build the examples" OFF)
option(OSSIACO_CONVERTER_BOOST_FS "Use Boost.Filesystem in unit test utils" OFF)

option(OSSIACO_WCHAR_UNICODE "whether to enable wchar_t as the primary character/string element type for using unicode on windows" OFF)
if(${OSSIACO_WCHAR_UNICODE})
    set(_ossiaco_wchar_unicode 1)
else()
    set(_ossiaco_wchar_unicode 0)
endif()

####################################
# Library dependency paths/packages
####################################



find_path(DATE_INCLUDE_DIRS date/date.h)
find_path(RAPIDJSON_INCLUDE_DIRS rapidjson/rapidjson.h)

find_package(Catch2 2.4.1 REQUIRED)
find_package(range-v3 0.4.0 REQUIRED)

if(${OSSIACO_CONVERTER_TESTS} AND ${OSSIACO_CONVERTER_BOOST_FS})
    find_package(Boost 1.67.0 COMPONENTS filesystem REQUIRED)
else()
    find_package(Boost 1.67.0 REQUIRED)
endif()

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

target_include_directories(ossiaco_converter INTERFACE
    ${DATE_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
)

target_link_libraries(ossiaco_converter INTERFACE
    Boost::boost
    range-v3 meta
)

target_compile_features(ossiaco_converter INTERFACE cxx_std_17)

target_compile_definitions(ossiaco_converter INTERFACE
    OSSIACO_WCHAR_UNICODE=${_ossiaco_wchar_unicode}
)

target_compile_definitions(ossiaco_converter INTERFACE
    RAPIDJSON_HAS_STDSTRING
)

if(MSVC)
    target_compile_options(ossiaco_converter INTERFACE
        /permissive-
        /bigobj
        /EHsc
    )
endif()

##############################
# The unit tests and examples
##############################

if(${OSSIACO_CONVERTER_EXAMPLES} OR ${OSSIACO_CONVERTER_TESTS})
    enable_testing()
endif()

if(${OSSIACO_CONVERTER_TESTS})
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif()

if(${OSSIACO_CONVERTER_EXAMPLES})
    add_subdirectory(examples)
endif()
