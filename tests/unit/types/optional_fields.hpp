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

using namespace Ossiaco::converter;

struct SilentOptionalFields {
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        SilentOptionalFields,
        (&SilentOptionalFields::_name, OSSIACO_XPLATSTR("name"))
        (&SilentOptionalFields::_i, OSSIACO_XPLATSTR("i"))
        (&SilentOptionalFields::_d, OSSIACO_XPLATSTR("d")));

    string_t _name;
    std::optional<int> _i;
    std::optional<double> _d;
};

struct NullOptionalFields {
    static constexpr auto nvp = NullValuePolicy::write;
    static constexpr auto nf = throwNotFound<NullOptionalFields>;

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        NullOptionalFields,
        (&NullOptionalFields::_name, OSSIACO_XPLATSTR("name"))
        (jsonProperty<nvp, nf>(&NullOptionalFields::_city, OSSIACO_XPLATSTR("city")))
        (jsonProperty<nvp, nf>(&NullOptionalFields::_light, OSSIACO_XPLATSTR("light")))
    );

    string_t _name;

    std::optional<string_t> _city;

    std::optional<Light> _light;
};

inline bool operator==(const NullOptionalFields& lhs, const NullOptionalFields& rhs)
{
    return std::tie(lhs._name, lhs._city, lhs._light) == std::tie(rhs._name, rhs._city, rhs._light);
}

inline bool operator==(const SilentOptionalFields& lhs, const SilentOptionalFields& rhs)
{
    return std::tie(lhs._name, lhs._i, lhs._d) == std::tie(rhs._name, rhs._i, rhs._d);
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPTIONAL_FIELDS_HPP
