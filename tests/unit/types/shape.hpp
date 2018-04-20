// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SHAPE_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SHAPE_HPP

#include "point_3d.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/decorator.hpp>

namespace test_types {

// This class is adapted from the CPP core guidelines C.129 on
// implementation vs interface inheritance.
class Shape {
public:
    virtual Point3D center() const = 0;

    OSSIACO_CONVERTER_BASE_SUPPORTED(Shape, ());

    virtual ~Shape() {}
};

class Circle : public Shape {
public:
    Circle(Point3D center, double radius) : _center(std::move(center)), _radius(radius) {}

    Circle()              = default;
    Circle(const Circle&) = default;
    Circle(Circle&&)      = default;

    Circle& operator=(const Circle&) = default;
    Circle& operator=(Circle&&) = default;

    ~Circle() override = default;

    Point3D center() const override { return _center; }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        Circle, Shape,
        (&Circle::_center, OSSIACO_XPLATSTR("center"))
        (&Circle::_radius, OSSIACO_XPLATSTR("radius")));

    friend bool operator==(const Circle& lhs, const Circle& rhs)
    {
        return std::tie(lhs._center, lhs._radius) == std::tie(rhs._center, rhs._radius);
    }

private:
    Point3D _center{};
    double _radius{};
};

class Segment : public Shape {
public:
    Segment(Point3D p1, Point3D p2) : _p1(p1), _p2(p2) {}

    Segment()               = default;
    Segment(const Segment&) = default;
    Segment(Segment&&)      = default;

    Segment& operator=(const Segment&) = default;
    Segment& operator=(Segment&&) = default;

    ~Segment() override = default;

    Point3D center() const override
    {
        return Point3D{(_p1._x - _p2._x) / 2, (_p1._y - _p2._y) / 2, (_p1._z - _p2._z) / 2};
    }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        Segment, Shape,
        (&Segment::_p1, OSSIACO_XPLATSTR("p1"))
        (&Segment::_p2, OSSIACO_XPLATSTR("p2")));

    friend bool operator==(const Segment& lhs, const Segment& rhs)
    {
        return std::tie(lhs._p1, lhs._p2) == std::tie(rhs._p1, rhs._p2);
    }

private:
    Point3D _p1{};
    Point3D _p2{};
};

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SHAPE_HPP
