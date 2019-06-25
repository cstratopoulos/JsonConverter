// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include "opaque.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/components/json_property.hpp>

namespace test_types {

namespace Oc = Ossiaco::converter;

constexpr auto Opaque::jsonProps() noexcept
{
    return Oc::PropertiesHelper<>{}(&Opaque::i, TSTR("i"))(&Opaque::d, TSTR("d"))._props;
}

Opaque::Opaque(const rapidjson::GenericValue<Oc::utf_t>& jv)
{
    Oc::ReferenceMapper refs;
    boost::hana::for_each(jsonProps(), [&, this](auto prop) { prop.fromJson(*this, jv, refs); });
}

void Opaque::toJson(Oc::PrettyStringBufferWriter& w) const
{
    Oc::ReferenceMapper refs;

    w.StartObject();
    boost::hana::for_each(jsonProps(), [&, this](auto prop) { prop.toJson(*this, w, refs); });
    w.EndObject();
}

} // namespace test_types
