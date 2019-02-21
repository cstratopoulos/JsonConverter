# Ossiaco JSON Converter Library
#
# Copyright (C) 2018-2019 Ossiaco
#
# Licensed under the MIT license ("the license"); you may not use this file
# except in compliance with the license. The license can be found in the root
# directory of this project, or at
#
# http://opensource.org/licenses/MIT

include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
check_cxx_source_compiles("int main() { return 0; }" COMPILER_HAS_UBSAN)

if(NOT ${COMPILER_HAS_UBSAN})
    message(FATAL_ERROR "Requested UBASAN on a compiler with no UB sanitizer")
endif()

set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
check_cxx_source_compiles("int main() { return 0; }" COMPILER_HAS_ASAN)

if(NOT ${COMPILER_HAS_ASAN})
    message(FATAL_ERROR "Requested UBASAN on a compiler with no address sanitizer")
endif()

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=address,undefined"
)
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined -fno-sanitize-recover=address,undefined"
)
