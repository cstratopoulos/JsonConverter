# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

##############################################################
# Tests of polymorphic serialization with an enum type field.
##############################################################

add_executable(enum_mapped_tests
    main.cpp
    single_level.cpp
    two_level.cpp
)

target_link_libraries(enum_mapped_tests
    PUBLIC converter_test_utils
)

add_test(
    NAME enum_mapped
    COMMAND enum_mapped_tests
)