// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_LIGHT_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_LIGHT_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/core/char_types.hpp>

#include <tuple>

namespace test_types {

struct Light {
    enum class Color { white, red, green, blue };

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Light,
        (&Light::_color, TSTR("color"))
        (&Light::_on,    TSTR("on")));

    Color _color{Color::white};
    bool _on{false};
};

inline bool operator==(const Light& lhs, const Light& rhs)
{
    return std::tie(lhs._color, lhs._on) == std::tie(rhs._color, rhs._on);
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_LIGHT_HPP
