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

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/decorator.hpp>

#include <catch2/catch.hpp>

namespace Oc = Ossiaco::converter;

struct X {
    X(int i, double d, Oc::string_t s)
        : _i(i), _d(d), _s(std::move(s))
    {}

    virtual ~X() = default;

    int _i;
    double _d;
    Oc::string_t _s;

    OSSIACO_CONVERTER_BASE_SUPPORTED(
        X,
        (requiredProperty(&X::_i, TSTR("i")),
         requiredProperty(&X::_d, TSTR("d")),
         requiredProperty(&X::_s, TSTR("s"))));
};

struct Y : public X {
    Y(X x, int j, double e)
        : X(std::move(x)), _j(j), _e(e)
    {}

    ~Y() override = default;

    int _j;
    double _e;

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        Y, X, 
        (requiredProperty(&Y::_j, TSTR("j")), 
         requiredProperty(&Y::_e, TSTR("e"))));
};

TEST_CASE("Struct adapted in make_tuple format", "[SimpleTypeAllocator]")
{
    X x(12, 345.6, TSTR("seven"));

    jsonCompare(
        Oc::toJsonStringPretty(x),
        TSTR(R"--({
    "@type": "X", 
    "i": 12, 
    "d": 345.6, 
    "s": "seven"
})--"));

    Y y(x, 8, 910.11);

    jsonCompare(
        Oc::toJsonStringPretty(y),
        TSTR(R"--({
    "@type": "Y", 
    "i": 12, 
    "d": 345.6, 
    "s": "seven", 
    "j": 8, 
    "e": 910.11
})--"));
}
