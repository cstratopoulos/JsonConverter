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
#include "../types/nested_ranges.hpp"
#include "../types/udt_ranges.hpp"

#include <catch2/catch.hpp>

namespace tt = test_types;

TEST_CASE("Linear ranges of user-defined types", "[ConvertLinearRange][SimpleTypeAllocator]")
{
    using Color = tt::Light::Color;
    runTestCases(makeSimpleStringTest("Some silly struct with Points and Lights", [] {
        return tt::PointsLights{{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
                                {{Color::red, true},
                                 {Color::green, false},
                                 {Color::white, false},
                                 {Color::blue, true}}};
    }));
}

TEST_CASE("Linear and Map ranges with nested ranges", "[ConvertLinearRange][ConvertKeyValueRange][SimpleTypeAllocator]")
{
    runTestCases(
        makeSimpleStringTest("A default constructed NumberMap", [] { return tt::NumberMap(); }),
        makeSimpleStringTest(
            "A NumberMap with some primes",
            [] {
                return tt::NumberMap(std::set<unsigned>({2, 3, 5, 11, 13}));
            }),
        makeSimpleStringTest("A ridiculous struct with nested maps", [] {
            tt::MapOfMaps result;
            auto& mapMap = result._mapMap;

            mapMap[OSSIACO_XPLATSTR("Animal noises")] = {
                {OSSIACO_XPLATSTR("Dog"),
                 {OSSIACO_XPLATSTR("woof"), OSSIACO_XPLATSTR("bark"), OSSIACO_XPLATSTR("grr")}},
                {OSSIACO_XPLATSTR("Cat"),
                 {OSSIACO_XPLATSTR("meow"), OSSIACO_XPLATSTR("purr"), OSSIACO_XPLATSTR("hiss")}},
                {OSSIACO_XPLATSTR("Bird"), {OSSIACO_XPLATSTR("chirp"), OSSIACO_XPLATSTR("tweet")}}};

            mapMap[OSSIACO_XPLATSTR("Vehicle parts")] = {{OSSIACO_XPLATSTR("Car"),
                                                          {OSSIACO_XPLATSTR("wheels"),
                                                           OSSIACO_XPLATSTR("bumper"),
                                                           OSSIACO_XPLATSTR("steering wheel"),
                                                           OSSIACO_XPLATSTR("gas tank")}},
                                                         {OSSIACO_XPLATSTR("Plane"),
                                                          {OSSIACO_XPLATSTR("wings"),
                                                           OSSIACO_XPLATSTR("engine"),
                                                           OSSIACO_XPLATSTR("landing gear")}}};

            return result;
        }));
}
