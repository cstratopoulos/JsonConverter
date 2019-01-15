# Ossiaco JSON Converter Library
# 
# Copyright (C) 2018 Ossiaco
# 
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
# 
# http://opensource.org/licenses/MIT

############################################################################
# Tests of polymorphic JSON conversion based on class which enable JSON 
# support using OSSIACO_CONVERTER_BASE_SUPPORTED macro for type-decorator
# conversion
############################################################################

add_executable(poly_decorator_tests
    abstract_base.cpp
    adapt_make_tuple.cpp
)

target_link_libraries(poly_decorator_tests
    PUBLIC converter_test_utils
)

add_test(
    NAME poly_decorator
    COMMAND poly_decorator_tests
)