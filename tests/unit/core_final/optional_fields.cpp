// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include "../types/optional_fields.hpp"
#include "../test_utils.hpp"

#include <ossiaco/converter/core/traits.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <boost/mp11/integral.hpp>

#include <catch2/catch.hpp>

namespace tt = test_types;

namespace Oc = Ossiaco::converter;

using namespace boost::mp11;

template<typename JsonProp, typename NullValuePolicy>
using ExpectedNullValPolicy = mp_bool<JsonProp::nullValuePolicy == NullValuePolicy::value>;

TEST_CASE(
    "Silent null std::optional fields",
    "[ConvertVocabType][optional][SimpleTypeAllocator][NullValuePolicy]")
{
    SECTION("Static asserts")
    {
        STATIC_REQUIRE(
            mp_all_of_q<
                Oc::traits::JsonPropertiesType<tt::SilentOptionalFields>,
                mp_bind_back<
                    ExpectedNullValPolicy,
                    std::integral_constant<Oc::NullValuePolicy, Oc::NullValuePolicy::silent>>>());
    }

    SECTION("JSON string appearance")
    {
        jsonCompare(
            Oc::toJsonStringPretty(tt::SilentOptionalFields{TSTR("garfield"), {}, {}}),
            TSTR(R"--({"name": "garfield"})--"));

        jsonCompare(
            Oc::toJsonStringPretty(tt::SilentOptionalFields{TSTR("jeffrey"), {}, 5.0}),
            TSTR(R"--({"name": "jeffrey", "d": 5.0})--"));
    }

    SECTION("Deserialization")
    {
        SECTION("Absent or null optional fields")
        {
            for (Oc::string_t jsonStr :
                 {TSTR(R"--({"name": "heathcliff", "i": 123 })--"),
                  TSTR(R"--({"name": "heathcliff", "i": 123, "d": null })--")}) {
                std::unique_ptr<tt::SilentOptionalFields> s;

                REQUIRE_NOTHROW([&] {
                    s = Oc::JsonDeserializer<tt::SilentOptionalFields>::fromString(jsonStr.data());
                }());

                REQUIRE(s);
                CHECK(*s == tt::SilentOptionalFields{TSTR("heathcliff"), 123, {}});
            }
        }

        SECTION("Absent required field")
        {
            Oc::string_t nameMissing(TSTR(R"--({"i": 1, "d": 2.0})--"));

            REQUIRE_THROWS_AS(
                Oc::JsonDeserializer<tt::SilentOptionalFields>::fromString(nameMissing.data()),
                Oc::RequiredPropertyMissing<tt::SilentOptionalFields>);
        }
    }

    SECTION("Conversion")
    {
        runTestCases(
            makeSimpleStringTest(
                "All optional fields present",
                [] {
                    return tt::SilentOptionalFields{TSTR("Gary"), 154, 12.34};
                }),
            makeSimpleStringTest(
                "No optional fields present",
                [] {
                    return tt::SilentOptionalFields{TSTR("Patrick"), {}, {}};
                }),
            makeSimpleStringTest("Only one optional field present", [] {
                return tt::SilentOptionalFields{TSTR("Sandy"), {}, 31.23403};
            }));
    }
}

TEST_CASE(
    "A type with write null optional UDT and primitive convertible fields",
    "[ConvertVocabType][optional][SimpleTypeAllocator][NullValuePolicy]")
{
    SECTION("Static asserts")
    {
        STATIC_REQUIRE(
            mp_all_of_q<
                mp_rest<Oc::traits::JsonPropertiesType<tt::NullOptionalFields>>,
                mp_bind_back<
                    ExpectedNullValPolicy,
                    std::integral_constant<Oc::NullValuePolicy, Oc::NullValuePolicy::write>>>());
    }

    SECTION("JSON string appearance")
    {
        jsonCompare(
            Ossiaco::converter::toJsonStringPretty(
                tt::NullOptionalFields{TSTR("Snoopy"), {}, {}}),
            TSTR(R"--({
"name": "Snoopy",
"city": null,
"light": null
})--"));

        jsonCompare(
            Ossiaco::converter::toJsonStringPretty(tt::NullOptionalFields{
                TSTR("Blossom"), TSTR("Townsville"), {}}),
            TSTR(R"--({
"name": "Blossom",
"city": "Townsville",
"light": null
})--"));
    }

    SECTION("An omitted optional field")
    {
        namespace Oc = Ossiaco::converter;

        // The std::optional field "light" is missing.
        Oc::string_t jsonString(TSTR(
            R"--(
{
"name": "Terry",
"city": "Paris"
})--"));
        CAPTURE(jsonString);
        CHECK_THROWS_AS(
            Oc::JsonDeserializer<tt::NullOptionalFields>::fromString(jsonString.data()),
            Oc::RequiredPropertyMissing<tt::NullOptionalFields>);
    }

    SECTION("Conversion")
    {

        runTestCases(
            makeSimpleStringTest(
                "All optional fields present",
                [] {
                    return tt::NullOptionalFields{TSTR("Christos"),
                                                  TSTR("MTL"),
                                                  tt::Light{tt::Light::Color::blue, true}};
                }),
            makeSimpleStringTest(
                "No optional fields present",
                [] {
                    return tt::NullOptionalFields{TSTR("Alex"), {}, {}};
                }),
            makeSimpleStringTest("One optional field present", [] {
                return tt::NullOptionalFields{
                    TSTR("Dog"), TSTR("London"), {}};
            }));
    }
}
