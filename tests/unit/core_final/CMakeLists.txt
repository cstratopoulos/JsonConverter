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
# Tests of core converter functionality based on classes which enable JSON
# support using the OSSIACO_CONVERTER_FINAL_SUPPORTED macro
############################################################################

add_executable(core_final_tests
    primitive.cpp
    ranges.cpp
    smart_ptr.cpp
    unilateral_to_json.cpp
    vocab_types.cpp
)

target_link_libraries(core_final_tests
    PUBLIC converter_test_utils
)

add_test(
    NAME core_final
    COMMAND core_final_tests
)