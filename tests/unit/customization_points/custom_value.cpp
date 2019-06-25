// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include "../test_utils.hpp"
#include "../types/boost_uuid.hpp"

#include <boost/uuid/string_generator.hpp>

#include <catch2/catch.hpp>

namespace tt = test_types;

TEST_CASE("Using ConvertCustomValue to convert boost UUIDs", "[ConvertCustomValue]")
{
    runTestCases(
        makeSimpleStringTest("A Key with a nil UUID", [] { return tt::UuidKey{}; }),
        makeSimpleStringTest("A Key with a string UUID", [] {
            return tt::UuidKey{boost::uuids::string_generator()(
                TSTR("01234567-89ab-cdef-0123-456789abcdef"))};
        }));
}
