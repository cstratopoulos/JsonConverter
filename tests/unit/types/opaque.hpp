// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPAQUE_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPAQUE_HPP

#include <ossiaco/converter/core/buffers_streams_writers.hpp>

#include <rapidjson/document.h>

namespace test_types {

struct Opaque {
    Opaque() = default;
    Opaque(const rapidjson::GenericValue<Ossiaco::converter::utf_t>&);

    void toJson(Ossiaco::converter::PrettyStringBufferWriter&) const;

    static constexpr auto jsonProps() noexcept;

    int i;
    double d;
};


} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_OPAQUE_HPP
