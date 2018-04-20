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

#include <catch.hpp>

#include <iostream>

namespace Oc = Ossiaco::converter;

namespace mp11 = boost::mp11;

static unsigned registeredCount = 0;

namespace test_types {

// The expected use of this hook is to log registration events but instead we just
// mutate a static variable so we verify it's been called the right number of times.
template<typename Derived>
void hookOssiacoConverterDecoratorLog(test_types::Shape*, Derived*)
{
    std::cout << "Registering " << boost::core::demangle(typeid(Derived).name()) << " into Shape\n";
    ++registeredCount;
}

} // namespace test_types

template<typename Derived>
using ShapeHookDetected = mp11::mp_bool<Oc::decoratorHookDetected<test_types::Shape, Derived>>;

/*
TEST_CASE(
    "Checking invocation of ADL hook for poly decorator registration",
    "[hooks][PolyDecoratorAllocator]")
{
    namespace tt = test_types;

    using DerivedList = mp11::mp_list<tt::Circle, tt::DrawableCircle, tt::Segment>;

    static_assert(mp11::mp_all_of<DerivedList, ShapeHookDetected>());
    SUCCEED("static_assert passed: All derived classes detected hook for registration with shape");

    mp11::mp_for_each<DerivedList>([](auto shapeDerived) {
        using DerivedShape = decltype(shapeDerived);

        REQUIRE(Oc::jsonPolyImpl<DerivedShape>());
    });

    CHECK(registeredCount == mp11::mp_size<DerivedList>());
}
*/
