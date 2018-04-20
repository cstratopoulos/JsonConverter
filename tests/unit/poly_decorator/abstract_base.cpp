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
#include "../types/shape.hpp"

#include <catch.hpp>

namespace tt = test_types;

template<typename Func>
auto makeShapeTest(std::string_view desc, Func&& func)
{
    return makeBasicTestCase<tt::Shape>(
        desc,
        std::forward<Func>(func),
        makeObjectComparison<tt::Shape>(),
        StringObjectConversion{});
}

TEST_CASE("Converting inheritors of a pure interface ABC", "[PolyDecoratorAllocator]")
{
    runTestCases(
        makeShapeTest("A default constructed Circle", [] { return tt::Circle(); }),
        makeShapeTest("A unit circle", [] { return tt::Circle(tt::Point3D{}, 1.0); }),
        makeShapeTest("A line segment", [] {
            return tt::Segment(tt::Point3D{0.0, 26.0, -13.0}, tt::Point3D{1.0, 939293.4, 332.2353});
        }));
}
