// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
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

/// Convert a type that is "first-class" convertible: primitive convertible or macro-enabled
template<typename Class, typename Enable = void>
struct ConvertFirstClass;

// Convert types with macro-enabled JSON conversion.
template<typename Class>
struct ConvertFirstClass<Class, std::enable_if_t<traits::supportsJsonConverter<Class>>> {

    template<typename Encoding>
    static void FromJson(
        Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
    {
        JsonDeserializer<Class, Encoding>::FromJson(object, jsonValue, refs);
    }

    template<typename Writer>
    static void ToJson(const Class& object, Writer& writer, ReferenceMapper& refs)
    {
        JsonSerializer<Class>::template ToJson<Writer>(object, writer, refs, nullptr);
    }
};

// Convert primitive types.
template<typename Class>
struct ConvertFirstClass<
    Class,
    std::enable_if_t<
        // We explicitly forbid detection of JsonConverter<Class> to handle the following
        // lamentable situation in which JsonConverter<X> is detected but std::string is also
        // constructible from X, making it primitiveConvertible.
        // struct X { operator const char*() { return "uh oh"; } JSON_CONVERTER_SUPPORTED(...) };
        traits::primitiveConvertible<Class> && !traits::supportsJsonConverter<Class>>> {

    template<typename Encoding>
    static void
    FromJson(Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper&)
    {
        object = getValue<Class>(jsonValue);
    }

    template<typename Writer>
    static void ToJson(const Class& object, Writer& writer, ReferenceMapper&)
    {
        writeValue(writer, object);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP
