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
#include "../types/sprint_record.hpp"

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>

#include <catch2/catch.hpp>

namespace tt = test_types;

TEST_CASE(
    "A type with optional UDT and primitive convertible fields", "[ConvertVocabType][optional][SimpleTypeAllocator]")
{
    SECTION("JSON string appearance") {
        tt::OptionalFields snoopy{OSSIACO_XPLATSTR("Snoopy"), {}, {}};
        jsonCompare(Ossiaco::converter::toJsonStringPretty(snoopy), OSSIACO_XPLATSTR(R"--({
"name": "Snoopy",
"city": null,
"light": null
})--"));
    }

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

    SECTION("An omitted optional field")
    {
        namespace Oc = Ossiaco::converter;

        // The std::optional field "light" is missing.
        const Oc::CharType* jsonString = OSSIACO_XPLATSTR(
            R"--(
{
"name": "Terry",
"city": "Paris"
})--");
        CAPTURE(jsonString);
        CHECK_THROWS_AS(
            Oc::JsonDeserializer<tt::OptionalFields>::fromString(jsonString),
            Oc::RequiredPropertyMissing<tt::OptionalFields>);
    }
}

TEST_CASE(
    "Converting sprint records with duration and DOB",
    "[date][chrono][ConvertVocabType][ChronoPropertyConverter][SimpleTypeAllocator]")
{
    using namespace boost::mp11;

    using namespace std::literals::chrono_literals;
    using namespace date::literals;

    namespace Oc = Ossiaco::converter;

    mp_for_each<mp_list<mp_false, mp_true>>([](auto Bool) {
        static constexpr bool pretty = decltype(Bool)::value;

        const auto testName = [=](std::string_view sprinter) {
            std::ostringstream result;
            result << "Converting " << sprinter << "'s 100m record "
                   << (pretty ? "(pretty)" : "(terse)");

            return result.str();
        };

        const auto usain    = testName("Usain Bolt");
        const auto florence = testName("Florence Griffith-Joyner");

        runTestCases(
            makeSimpleStringTest(
                usain,
                [] {
                    return tt::SprintRecord<pretty>(
                        OSSIACO_XPLATSTR("Usain Bolt"), 1986_y / date::aug / 21, 9580ms);
                }),
            makeSimpleStringTest(florence, [] {
                return tt::SprintRecord<pretty>(
                    OSSIACO_XPLATSTR("Florence Griffith-Joyner"), 1959_y / date::dec / 21, 10490ms);
            }));
    });
}
