# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

option(OSSIACO_CONVERTER_TESTS "Build the unit/integration tests" OFF)
option(OSSIACO_CONVERTER_EXAMPLES "Build the examples" OFF)
option(OSSIACO_CONVERTER_BOOST_FS "Use Boost.Filesystem in unit test utils" OFF)
option(OSSIACO_WCHAR_UNICODE "Enable wchar_t as the primary character/string element type for using unicode on windows" OFF)
option(OSSIACO_CONVERTER_UBASAN "Enable undefined behavior and address sanitizers" OFF)

if(${OSSIACO_WCHAR_UNICODE})
    set(_ossiaco_wchar_unicode 1)
else()
    set(_ossiaco_wchar_unicode 0)
endif()

if(${OSSIACO_CONVERTER_UBASAN})
    include(converter_ubasan)
endif()
