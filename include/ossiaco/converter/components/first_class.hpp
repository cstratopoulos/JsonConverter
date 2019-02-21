// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP
#define OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP

#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/serialize/deserializer.hpp>
#include <ossiaco/converter/serialize/serializer.hpp>

#include <rapidjson/document.h>

#include <type_traits>

namespace Ossiaco::converter {

class ReferenceMapper;

template<typename Class>
struct ConvertFirstClass {
    static_assert(traits::jsonSupportDetected<Class> || traits::primitiveConvertible<Class>);

    template<typename Encoding>
    static void fromJson(
        Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
    {
        if constexpr (traits::jsonSupportDetected<Class>)
            JsonDeserializer<Class, Encoding>::fromJson(object, jsonValue, refs);
        else
            object = getValue<Class>(jsonValue);
    }

    template<typename Writer>
    static void toJson(const Class& object, Writer& writer, ReferenceMapper& refs)
    {
        if constexpr (traits::jsonSupportDetected<Class>)
            JsonSerializer<Class>::template toJson<Writer>(object, writer, refs, nullptr);
        else
            writeValue(writer, object);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP
