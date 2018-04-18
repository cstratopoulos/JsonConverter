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
#include "types/point_3d.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>

#include <catch.hpp>

#include <tuple>

namespace tt = test_types;

TEST_CASE("Converting 3D points", "[final]") 
{
    runTestCases(
        makeSimpleStringTest("The origin", [] { return tt::Point3D{}; }),
        makeSimpleStringTest(
            "A unit vector",
            [] {
                return tt::Point3D{0.0, 1.0, 0.0};
            }),
        makeSimpleStringTest("Some arbitrary vector", [] {
            return tt::Point3D{-1.5, 1000.12345, -99.87654321};
        }));
}
