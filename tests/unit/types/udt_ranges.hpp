// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_UDT_RANGES_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_UDT_RANGES_HPP

#include "light.hpp"
#include "point_3d.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/components/ranges.hpp>

#include <boost/container/small_vector.hpp>

#include <tuple>
#include <vector>

namespace test_types {

struct PointsLights {
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        PointsLights,
        (&PointsLights::_points, OSSIACO_XPLATSTR("points"))
        (&PointsLights::_lights, OSSIACO_XPLATSTR("lights")));

    boost::container::small_vector<Point3D, 5> _points{};

    std::vector<Light> _lights{};
};

inline bool operator==(const PointsLights& lhs, const PointsLights& rhs)
{
    return std::tie(lhs._points, lhs._lights) == std::tie(rhs._points, rhs._lights);
}

inline bool operator!=(const PointsLights& lhs, const PointsLights& rhs)
{
    return !(lhs == rhs);
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_UDT_RANGES_HPP
