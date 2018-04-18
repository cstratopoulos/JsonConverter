// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Testing structs/classes with OSSIACO_CONVERTER_FINAL_SUPPORTED conversion support and
// only primitive-convertible members

#include "test_utils.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>

#include <catch.hpp>

#include <tuple>

struct Point3D {
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Point3D,
        (&Point3D::_x, OSSIACO_XPLATSTR("x"))
        (&Point3D::_y, OSSIACO_XPLATSTR("y"))
        (&Point3D::_z, OSSIACO_XPLATSTR("z")));

    double _x{0.0};
    double _y{0.0};
    double _z{0.0};
};

bool operator==(const Point3D& p1, const Point3D& p2)
{
    return std::tie(p1._x, p1._y, p1._z) == std::tie(p2._x, p2._y, p2._z);
}

bool operator!=(const Point3D& p1, const Point3D& p2)
{
    return !(p1 == p2);
}

TEST_CASE("Converting 3D points", "[final]") 
{
    runTestCases(
        makeSimpleStringTest("The origin", [] { return Point3D{}; }),
        makeSimpleStringTest(
            "A unit vector",
            [] {
                return Point3D{0.0, 1.0, 0.0};
            }),
        makeSimpleStringTest("Some arbitrary vector", [] {
            return Point3D{-1.5, 1000.12345, -99.87654321};
        }));
}
