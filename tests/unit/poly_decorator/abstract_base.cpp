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

struct OopsShape : tt::Shape {
    ~OopsShape() override {}

    tt::Point3D center() const override { return {}; }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        OopsShape, test_types::Shape, (&OopsShape::_someInt, OSSIACO_XPLATSTR("someInt")));

    int _someInt{};
};

TEST_CASE("Converting inheritors of a pure interface ABC", "[PolyDecoratorAllocator]")
{
    runTestCases(
        makeShapeTest("A default constructed Circle", [] { return tt::Circle(); }),
        makeShapeTest("A unit circle", [] { return tt::Circle(tt::Point3D{}, 1.0); }),
        makeShapeTest("A line segment", [] {
            return tt::Segment(tt::Point3D{0.0, 26.0, -13.0}, tt::Point3D{1.0, 939293.4, 332.2353});
        }));
}

TEST_CASE("Type decorator allocation errors", "[PolyDecoratorAllocator]")
{
    namespace Oc = Ossiaco::converter;

    SECTION("Trying to allocate an abstract type")
    {
        const auto jsonString = OSSIACO_XPLATSTR(
            R"--(
{
"@type": "test_types::Shape"
}
)--");
        CAPTURE(jsonString);
        CHECK_THROWS_AS(
            Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
            Oc::AbstractTypeAllocation<tt::Shape>);
    }

    SECTION("Unregistered type errors")
    {
        SECTION("Trying to deserialize from a class which is not derived from base (and does not exist!)")
        {
            const auto jsonString = OSSIACO_XPLATSTR(
                R"--(
{
"@type": "some_namespace::SomeClass",
"field": "value"
}
)--");
            CAPTURE(jsonString);

            CHECK_THROWS_AS(
                Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
                Oc::UnregisteredType<tt::Shape>);
        }

        SECTION("Trying to deserialize from a genuine inheritor which has not been registered")
        {
            OopsShape oops{};
            const auto jsonString = Oc::toJsonStringPretty(oops);
            CAPTURE(jsonString);

            CHECK_THROWS_AS(
                Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
                Oc::UnregisteredType<tt::Shape>);

            CHECK(Oc::jsonPolyImpl<OopsShape>());

            CHECK_NOTHROW(Oc::JsonDeserializer<tt::Shape>::fromString(jsonString));
        }
    }
}