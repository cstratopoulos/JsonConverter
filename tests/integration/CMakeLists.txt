# Ossiaco JSON Converter Library
#
# Copyright (C) 2018-2019 Ossiaco
#
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
#
# http://opensource.org/licenses/MIT

# The integration test suite consists purely of isolated .cpp files
# which include one of the catch-all headers from ossiaco/converter
# to ensure they can be compiled alone with no errors.

add_executable(converter_integration
    integration_main.cpp
    adapt.cpp
    allocate.cpp
    components.cpp
    serialize.cpp
)

target_link_libraries(converter_integration
    PUBLIC ossiaco_converter
)