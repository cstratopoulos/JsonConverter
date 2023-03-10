// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_POINT_3D_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_POINT_3D_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>

#include <tuple>

namespace test_types {

struct Point3D {
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Point3D,
        (&Point3D::_x, TSTR("x"))
        (&Point3D::_y, TSTR("y"))
        (&Point3D::_z, TSTR("z")));

    double _x{ 0.0 };
    double _y{ 0.0 };
    double _z{ 0.0 };
};

inline bool operator==(const Point3D& p1, const Point3D& p2)
{
    return std::tie(p1._x, p1._y, p1._z) == std::tie(p2._x, p2._y, p2._z);
}

inline bool operator<(const Point3D& p1, const Point3D& p2)
{
    return std::tie(p1._x, p1._y, p1._z) < std::tie(p2._x, p2._y, p2._z);
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_POINT_3D_HPP
