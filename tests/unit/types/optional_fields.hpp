// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPTIONAL_FIELDS_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPTIONAL_FIELDS_HPP

#include "light.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/components/vocab_type.hpp>

#include <optional>

namespace test_types {

struct OptionalFields {
    using string_t = Ossiaco::converter::string_t;

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        OptionalFields,
        (&OptionalFields::_name, OSSIACO_XPLATSTR("name"))
        (&OptionalFields::_city, OSSIACO_XPLATSTR("city"))
        (&OptionalFields::_light, OSSIACO_XPLATSTR("light")));

    string_t _name;

    std::optional<string_t> _city;

    std::optional<Light> _light;
};

inline bool operator==(const OptionalFields& lhs, const OptionalFields& rhs)
{
    return std::tie(lhs._name, lhs._city, lhs._light) == std::tie(rhs._name, rhs._city, rhs._light);
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPTIONAL_FIELDS_HPP
