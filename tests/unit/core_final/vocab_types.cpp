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
#include "../types/optional_fields.hpp"

#include <catch.hpp>

namespace tt = test_types;

TEST_CASE("A type with optional UDT and primitive convertible fields", "[ConvertVocabType][optional]")
{
    runTestCases(
        makeSimpleStringTest(
            "All optional fields present",
            [] {
                return tt::OptionalFields{OSSIACO_XPLATSTR("Christos"),
                                          OSSIACO_XPLATSTR("MTL"),
                                          tt::Light{tt::Light::Color::blue, true}};
            }),
        makeSimpleStringTest(
            "No optional fields present",
            [] {
                return tt::OptionalFields{OSSIACO_XPLATSTR("Alex"), {}, {}};
            }),
        makeSimpleStringTest("One optional field present", [] {
            return tt::OptionalFields{OSSIACO_XPLATSTR("Dog"), OSSIACO_XPLATSTR("London"), {}};
        }));

    SECTION("An omitted optional field") {
        namespace Oc = Ossiaco::converter;

        // The std::optional field "light" is missing.
        const Oc::CharType* jsonString = OSSIACO_XPLATSTR(
            R"--(
{
"name": "Terry",
"city": "Paris"
})--");
        CHECK_THROWS_AS(Oc::JsonDeserializer<tt::OptionalFields>::fromString(jsonString), Oc::RequiredPropertyMissing<tt::OptionalFields>);
    }
}