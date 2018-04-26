// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Testing types with only primitive convertible members

#include "../test_utils.hpp"
#include "../types/light.hpp"
#include "../types/person.hpp"
#include "../types/point_3d.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>

#include <catch.hpp>

#include <tuple>

namespace tt = test_types;

TEST_CASE("Converting 3D points", "[double][SimpleTypeAllocator]")
{
    tt::Point3D unitVec{ 0.0, 1.0, 0.0 };

    SECTION("JSON string appearance")
    {
        jsonCompare(
            Ossiaco::converter::toJsonStringPretty(unitVec),
            OSSIACO_XPLATSTR(
                R"--({
"x": 0.0,
"y": 1.0,
"z": 0.0
})--"));
    }

    runTestCases(
        makeSimpleStringTest("The origin", [] { return tt::Point3D{}; }),
        makeSimpleStringTest(
            "A unit vector",
            [=] {
                return unitVec;
            }),
        makeSimpleStringTest("Some arbitrary vector", [] {
            return tt::Point3D{-1.5, 1000.12345, -99.87654321};
        }));
}

TEST_CASE("Converting Person (name/city of residence string)"
          "[string][unicode][SimpleTypeAllocator]")
{
    SECTION("JSON string appearance") {
        tt::Person marge(OSSIACO_XPLATSTR("Marge"), OSSIACO_XPLATSTR("Springfield"));
        jsonCompare(
            Ossiaco::converter::toJsonStringPretty(marge),
            OSSIACO_XPLATSTR(
                R"--({
"name": "Marge",
"city": "Springfield"
})--"));
    }

    runTestCases(
        makeSimpleStringTest(
            "A Person with all ASCII characters",
            [=] { return tt::Person(OSSIACO_XPLATSTR("Sam"), OSSIACO_XPLATSTR("Texas")); }),
        makeSimpleStringTest(
            "A Person with accented city name",
            [] { return tt::Person(OSSIACO_XPLATSTR("Christos"), OSSIACO_XPLATSTR("Montréal")); }),
        makeSimpleStringTest("A Person with a Japanese name", [] {
            return tt::Person(OSSIACO_XPLATSTR("あいこ"), OSSIACO_XPLATSTR("東京都"));
        }));
}

TEST_CASE("Converting Light (bool/enum members)", "[bool][enum][SimpleTypeAllocator]")
{
    SECTION("JSON string appearance") {
        tt::Light off{};
        jsonCompare(Ossiaco::converter::toJsonStringPretty(off), OSSIACO_XPLATSTR(R"--({
"color": 0,
"on": false
})--"));
    }

    runTestCases(
        makeSimpleStringTest("A default-constructed Light (white/off)", [] { return tt::Light{}; }),
        makeSimpleStringTest(
            "A red Light turned on",
            [] {
                return tt::Light{tt::Light::Color::red, true};
            }),
        makeSimpleStringTest("A light with an invalid enum value", [] {
            return tt::Light{static_cast<tt::Light::Color>(25), true};
        }));
}
