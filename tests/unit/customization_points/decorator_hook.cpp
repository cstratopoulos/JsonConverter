// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Testing the definition of an ADL hook to record when a type is registered for poly type
// decoration.

#include "../types/shape.hpp"

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/function.hpp>

#include <catch2/catch.hpp>

#include <iostream>

namespace Oc = Ossiaco::converter;

using namespace boost::mp11;

static std::size_t registeredCount = 0;

namespace test_types {

// The expected use of this hook is to log registration events but instead we just
// mutate a static variable so we verify it's been called the right number of times.
template<typename Base, typename Derived>
void hookOssiacoConverterDecoratorLog(Base*, Derived*)
{
    std::cout << "Registering " << boost::core::demangle(typeid(Derived).name()) << " into "
        << boost::core::demangle(typeid(Base).name()) << "\n";
    ++registeredCount;
}

} // namespace test_types

template<typename Derived>
using ShapeHookDetected = mp_bool<Oc::decoratorHookDetected<test_types::Shape, Derived>>;

template<typename Derived>
constexpr std::size_t inheritanceDepth()
{
    if constexpr(Oc::traits::jsonBaseDetected<Derived>)
        return 1 + inheritanceDepth<typename Derived::JsonBase>();
    else
        return 0;
}

template<typename Derived>
using InheritanceDepthType = mp_int<inheritanceDepth<Derived>()>;

TEST_CASE(
    "Checking invocation of ADL hook for poly decorator registration",
    "[hooks][PolyDecoratorAllocator]")
{
    namespace tt = test_types;

    using DerivedList = mp_list<tt::Circle, tt::DrawableCircle, tt::Segment, tt::Circle, tt::DrawableCircle>;

    STATIC_REQUIRE(mp_all_of<DerivedList, ShapeHookDetected>());

    mp_for_each<DerivedList>([](auto shapeDerived) {
        using DerivedShape = decltype(shapeDerived);

        REQUIRE(Oc::jsonPolyImpl<DerivedShape>());
    });

    // Given a hierarchy Base_0, Base_1, ..., Base_(n-1), Derived, registration of Derived into Base0 should happen precisely once
    // for each Base_k from 0 to n-1.
    using DepthCounts = mp_transform<
        InheritanceDepthType,
        mp_unique<DerivedList>>;

    // This is possible as mp_apply<mp_plus, DepthCounts> but MSVC doesn't like it!
    std::size_t expectedCount = 0;
    mp_for_each<DepthCounts>([&expectedCount](auto intC) { expectedCount += intC(); });

    CHECK(registeredCount == expectedCount);
}

