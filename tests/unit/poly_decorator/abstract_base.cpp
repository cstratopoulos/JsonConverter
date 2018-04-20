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

template<typename Base = tt::Shape, typename Func>
auto makeShapeTest(std::string_view desc, Func&& func)
{
    return makeBasicTestCase<Base>(
        desc,
        std::forward<Func>(func),
        makeObjectComparison<Base>(),
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

TEST_CASE("Converting through direct and indirect bases", "[PolyDecoratorAllocator]")
{
    const tt::DrawableCircle drawCirc(tt::Point3D{ 1.0, 2.0, 3.0 }, 4.0);

    runTestCases(
        makeShapeTest<tt::Shape>("DrawableCircle through Shape", [=] { return drawCirc; }),
        makeShapeTest<tt::Circle>("DrawableCircle through Circle", [=] { return drawCirc; }));
}

struct OopsShape : tt::Shape {
    ~OopsShape() override {}

    tt::Point3D center() const override { return {}; }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        OopsShape, test_types::Shape, 
        (&OopsShape::_someInt, OSSIACO_XPLATSTR("someInt")));

    int _someInt{};
};


SCENARIO("Raising allocation errors", "[PolyDecoratorAllocator]")
{
    namespace Oc = Ossiaco::converter;

    GIVEN("A JSON string describing an abstract type")
    {
        const auto jsonString = OSSIACO_XPLATSTR(
            R"--(
{
"@type": "test_types::Shape"
}
)--");

        THEN("Trying to deserialize it throws an abstract allocation error")
        {
            CAPTURE(jsonString);
            CHECK_THROWS_AS(
                Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
                Oc::AbstractTypeAllocation<tt::Shape>);
        }
    }

    GIVEN("A JSON string for a class which does not exist")
    {
        const auto jsonString = OSSIACO_XPLATSTR(
            R"--(
{
"@type": "some_namespace::SomeClass",
"field": "value"
}
)--");
        THEN("Trying to deserialize it through an abstract base throws unregistered type error")
        {
            CAPTURE(jsonString);

            CHECK_THROWS_AS(
                Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
                Oc::UnregisteredType<tt::Shape>);
        }
    }

    GIVEN("A JSON string for a derived class that is not registered through a base")
    {
        OopsShape oops{};
        const auto jsonString = Oc::toJsonStringPretty(oops);
        CAPTURE(jsonString);

        THEN("Trying to deserialize it through its base throws unregistered type error")
        {
            CHECK_THROWS_AS(
                Oc::JsonDeserializer<tt::Shape>::fromString(jsonString),
                Oc::UnregisteredType<tt::Shape>);

            AND_WHEN("We register it through its base")
            {
                REQUIRE(Oc::jsonPolyImpl<OopsShape>());

                THEN("Polymorphic deserialization throws no error")
                {
                    CHECK_NOTHROW(Oc::JsonDeserializer<tt::Shape>::fromString(jsonString));
                }
            }
        }
    }
}