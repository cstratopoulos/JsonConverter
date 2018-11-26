// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_PROPERTY_CONVERTER_HPP
#define OSSIACO_CONVERTER_COMPONENTS_PROPERTY_CONVERTER_HPP

#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/not_found.hpp>

#include <rapidjson/document.h>

namespace Ossiaco::converter {

class ReferenceMapper;

/// Conversion of a class's JSON property stored as a name string and PMD.
///
/// This class simply delegates to [DeducedConverterType], wrapping the common logic of checking for
/// the member existence, and printing the property name during serialization.
template<
    typename Class,
    typename Property,
    NotFoundHandlerPtr notFound = throwNotFound<Class>>
class PropertyConverter {
public:
    using ConvHandler = DeducedConverterType<Property>;
    using PmdType     = Property Class::*;

    constexpr PropertyConverter(PmdType member, const CharType* name) noexcept
        : _member(member), _name(name)
    {}

    template<typename Encoding>
    void fromJson(Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs) const
    {
        if (auto findItr = jsonValue.FindMember(_name); findItr != jsonValue.MemberEnd())
            ConvHandler::fromJson(object.*_member, findItr->value, refs);
        else
            notFound(_name);
    }

    template<typename Writer>
    void toJson(const Class& object, Writer& writer, ReferenceMapper& refs)
    {
        writer.String(_name);
        ConvHandler::toJson(object.*_member, writer, refs);
    }

private:
    const PmdType _member;

    const CharType* _name;
};

/// Alias template for a JSON property whose absence during deserialization should be ignored.
template<typename Class, typename Property>
using OptionalPropertyConverter = PropertyConverter<Class, Property, &ignoreNotFound>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_PROPERTY_CONVERTER_HPP
