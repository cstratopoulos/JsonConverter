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
# Tests of customization points available to library users.
# Includes hooks, policies, and non-intrusive conversion of existing types.
############################################################################

add_executable(customization_point_tests
    main.cpp
    custom_value.cpp
)

target_link_libraries(customization_point_tests
    PUBLIC converter_test_utils
)

add_test(
    NAME customization_points
    COMMAND customization_point_tests
)