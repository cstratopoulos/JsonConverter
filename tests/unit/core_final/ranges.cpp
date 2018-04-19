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

#include <catch.hpp>

namespace tt = test_types;

TEST_CASE("Linear ranges of user-defined types", "[ConvertLinearRange]")
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

TEST_CASE("Linear and Map ranges with nested ranges", "[ConvertLinearRange][ConvertKeyValueRange]")
{
    runTestCases(
        makeSimpleStringTest("A default constructed NumberMap", [] { return tt::NumberMap(); }),
        makeSimpleStringTest("A NumberMap with some primes", [] {
            return tt::NumberMap(std::set<unsigned>({2, 3, 5, 11, 13}));
        }));
}
