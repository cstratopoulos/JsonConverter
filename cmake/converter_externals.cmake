# Ossiaco JSON Converter Library
#
# Copyright (C) 2018-2019 Ossiaco
#
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
#
# http://opensource.org/licenses/MIT

find_path(DATE_INCLUDE_DIRS date/date.h)
find_path(RAPIDJSON_INCLUDE_DIRS rapidjson/rapidjson.h)

find_package(range-v3 0.9.1 REQUIRED)

if(${OSSIACO_CONVERTER_TESTS})
    find_package(Catch2 2.9 REQUIRED)
    if(${OSSIACO_CONVERTER_BOOST_FS})
        find_package(Boost 1.69.0 COMPONENTS filesystem REQUIRED)
    else()
        find_package(Boost 1.69.0 REQUIRED)
    endif()
endif()
