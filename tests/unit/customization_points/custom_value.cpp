// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include "../test_utils.hpp"
#include "../types/boost_uuid.hpp"

#include <boost/uuid/random_generator.hpp>

#include <catch.hpp>

namespace tt = test_types;

TEST_CASE("Using ConvertCustomValue to convert boost UUIDs", "[ConvertCustomValue]")
{
    constexpr bool converterDeduced = Ossiaco::converter::
        expectedConverterComponent<boost::uuids::uuid, Ossiaco::converter::ConvertCustomValue>;

    static_assert(converterDeduced);
    SUCCEED("Static converter dispatch as expected");

    runTestCases(
        makeSimpleStringTest("A Key with a nil UUID", [] { return tt::UuidKey{}; }),
        makeSimpleStringTest("A Key with a random UUID", [] {
            return tt::UuidKey{boost::uuids::random_generator()()};
        }));
}
