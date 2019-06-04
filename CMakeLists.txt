# Ossiaco JSON Converter Library
#
# Copyright (C) 2018-2019 Ossiaco
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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#######################
# Configuration macros
#######################

include(converter_options)

####################################
# Library dependency paths/packages
####################################

include(converter_externals)

###############################
# The converter library target
###############################

add_library(ossiaco_converter INTERFACE)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/chrono_fmt_json_property.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/custom_value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/dispatch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/first_class.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/json_property.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/ranges.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/smart_ptr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/template_get_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/components/vocab_type.hpp

    # Core definitions/classes
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/buffers_streams_writers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/char_types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/core/not_found.hpp
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
	${CMAKE_CURRENT_SOURCE_DIR}/include/ossiaco/converter/utils/accessors.hpp
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
    find_program(MEMORYCHECK_COMMAND valgrind)
    if(MEMORYCHECK_COMMAND)
        message(STATUS "Found valgrind: ${MEMORYCHECK_COMMAND}")
        set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
        include(CTest)
    else()
        message(STATUS "Valgrind not found")
    endif()
    enable_testing()
endif()

if(${OSSIACO_CONVERTER_TESTS})
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif()

if(${OSSIACO_CONVERTER_EXAMPLES})
    add_subdirectory(examples)
endif()
