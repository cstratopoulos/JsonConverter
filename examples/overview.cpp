// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include <ossiaco/converter/converter.hpp>

#include <iostream>
#include <tuple>
#include <utility>

#include <cassert>

// Define a simple struct for which we will enable JSON conversion
struct Point3D {
    // This macro is used to enable conversion for classes where we 
    // don't want to enable polymorphic conversion. 
    // Its first argument is...
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Point3D,                                // ...Class name, followed by...
        (&Point3D::_x, OSSIACO_XPLATSTR("x"))   // ...a series of operator() calls taking a member data pointer and
        (&Point3D::_y, OSSIACO_XPLATSTR("y"))   // string literal for the JSON property name. Note these are chained,
        (&Point3D::_z, OSSIACO_XPLATSTR("z"))); // *not* comma separated!

    // N.B.: The OSSIACO_XPLATSTR macro expands its argument verbatim or to a wide
    // character literal depending on the OSSIACO_WCHAR_UNICODE macro. This is
    // done for portability, you are free to exclude it in your code.

    double _x{0.0};
    double _y{0.0};
    double _z{0.0};
};

// Now let's define an abstract base class which will be the root of an 
// inheritance hierarchy.
struct Shape {
    Shape()             = default;
    Shape(const Shape&) = default;
    Shape(Shape&&)      = default;

    Shape& operator=(const Shape&) = default;
    Shape& operator=(Shape&&) = default;

    virtual ~Shape() = default;

    virtual Point3D center() const = 0;

    // This macro declares the base class of a JSON convertible polymorphic
    // hierarchy. Its format is much like the FINAL_SUPPORTED macro.
    OSSIACO_CONVERTER_BASE_SUPPORTED(
        Shape, ()); // Note that an empty () call is required for classes with no properties.
};

// Time to define some inheritors of Shape.

struct Segment : public Shape {
    Segment(Point3D p1, Point3D p2) : _p1(std::move(p1)), _p2(std::move(p2)) {}

    Segment()               = default;
    Segment(const Segment&) = default;
    Segment(Segment&&)      = default;

    Segment& operator=(const Segment&) = default;
    Segment& operator=(Segment&&) = default;

    ~Segment() = default;

    Point3D center() const override
    {
        return Point3D{
            (_p1._x - _p2._x) / 2,
            (_p1._y - _p2._y) / 2,
            (_p1._z - _p2._z) / 2
        };
    }

    // This macro is like the ones we've seen except for the extra type argument
    // which declares the JSON-convertible base class.
    OSSIACO_CONVERTER_POLY_SUPPORTED(
        Segment, Shape,
        (&Segment::_p1, OSSIACO_XPLATSTR("p1"))    // Note that p1 and p2 are instances of Point3D:
        (&Segment::_p2, OSSIACO_XPLATSTR("p2")));  // we can compose JSON-convertible types.

    Point3D _p1{};
    Point3D _p2{};
};

struct Circle : public Shape {
    Circle(Point3D center, double radius) : _center(std::move(center)), _radius(radius) {}

    Circle()              = default;
    Circle(const Circle&) = default;
    Circle(Circle&&)      = default;

    Circle& operator=(const Circle&) = default;
    Circle& operator=(Circle&&) = default;

    ~Circle() = default;

    Point3D center() const override { return _center; }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        Circle, Shape,
        (&Circle::_center, OSSIACO_XPLATSTR("center"))  
        (&Circle::_radius, OSSIACO_XPLATSTR("radius")));

    Point3D _center{};
    double _radius{0.0};
};

// Define some equality operators to verify conversion below.

bool operator==(const Point3D&, const Point3D&);
bool operator==(const Segment&, const Segment&);
bool operator==(const Circle&, const Circle&);

namespace Oc = Ossiaco::converter;

#if OSSIACO_WCHAR_UNICODE
auto& tcout = std::wcout;
#else
auto& tcout = std::cout;
#endif

int main()
{
    // Register derived classes of Shape for polymorphic conversion
    assert(Oc::jsonPolyImpl<Circle>());
    assert(Oc::jsonPolyImpl<Segment>());

    // Instantiate some classes, serialize them to JSON strings
    const Point3D yUnit{ 0.0, 1.0, 0.0 };
    const Circle unitCircle(yUnit, 1.0);

    const Oc::string_t pointStr  = Oc::toJsonStringPretty(yUnit);
    const Oc::string_t circleStr = Oc::toJsonStringPretty(unitCircle);

    tcout << pointStr << '\n';
    // Prints
    // {
    //     "x": 0.0,
    //     "y": 1.0,
    //     "z": 0.0
    // }

    // Deserialize a Point3D from a string.
    std::unique_ptr<Point3D> yUnit2 = Oc::JsonDeserializer<Point3D>::fromString(pointStr);
    assert(yUnit == *yUnit2);

    tcout << circleStr << '\n';
    // Prints
    // {
    //     "@type": "Circle",
    //     "center": {
    //         "x": 0.0,
    //         "y": 1.0,
    //         "z": 0.0
    //     }
    // }

    // We can deserialize directly to a Circle...
    std::unique_ptr<Circle> unitCircle2 = Oc::JsonDeserializer<Circle>::fromString(circleStr);
    assert(unitCircle == *unitCircle2);

    // ...or deserialize polymorphically into a Shape pointer.
    std::unique_ptr<Shape> circleShape = Oc::JsonDeserializer<Shape>::fromString(circleStr);
    const auto circleFromShape         = dynamic_cast<const Circle*>(circleShape.get());
    assert(circleFromShape);
    assert(unitCircle == *circleFromShape);

    // We can also do both serialization and deserialization polymorphically.
    const Segment seg(yUnit, {});
    const Oc::string_t segmentAsShapeStr = Oc::toJsonStringPretty(static_cast<const Shape&>(seg));

    tcout << segmentAsShapeStr << '\n';
    // Prints
    // {
    //     "@type": "Segment",
    //     "p1": {
    //         "x": 0.0,
    //         "y": 1.0,
    //         "z": 0.0
    //     },
    //     "p2": {
    //         "x": 0.0,
    //         "y": 0.0,
    //         "z": 0.0
    //     }
    // }

    std::unique_ptr<Shape> segmentShape = Oc::JsonDeserializer<Shape>::fromString(segmentAsShapeStr);
    const auto segmentFromShape         = dynamic_cast<const Segment*>(segmentShape.get());
    assert(segmentFromShape);
    assert(seg == *segmentFromShape);
}

bool operator==(const Point3D& p1, const Point3D& p2)
{
    return std::tie(p1._x, p1._y, p1._z) == std::tie(p2._x, p2._y, p2._z);
}

bool operator==(const Segment& s1, const Segment& s2)
{
    return std::tie(s1._p1, s1._p2) == std::tie(s2._p1, s2._p2);
}

bool operator==(const Circle &c1, const Circle &c2)
{
    return std::tie(c1._center, c1._radius) == std::tie(c2._center, c2._radius);
}