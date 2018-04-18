// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_PROP_CONVERTER_HPP
#define OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_PROP_CONVERTER_HPP

#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/not_found.hpp>

#include <boost/type_traits/is_detected.hpp>
#include <date/date.h>
#include <rapidjson/document.h>

#include <utility>

namespace Ossiaco::converter {

class ReferenceMapper;

/// Syntatctic "salt" alias which explicitly associates a chrono PMD to its format string.
template<typename Class, typename Property>
using ChronoFmtPair = std::pair<Property Class::*, const CharType*>;

/// Type deduction helper for [ChronoFmtPair].
template<typename Class, typename Property>
constexpr auto chronoFmtPair(Property Class::*member, const CharType* format)
{
    return std::make_pair(member, format);
}

/// Like [PropertyConverter], but for a date/chrono property with formatting governed by a format string.
///
/// Property must be any chrono-related type which can be streamed with the to_stream/from_stream API
/// of the date library. Conversion uses date::parse/date::format, potentially enabling ADL for user-defined
/// date types.
template<
    typename Class,
    typename Property,
    NotFoundHandlerPtr<Class> notFound = &throwNotFound<Class>>
class ChronoPropertyConverter {
public:
    using PmdType = Property Class::*;

    constexpr ChronoPropertyConverter(ChronoFmtPair<Class, Property> fmtPair, const CharType* name) noexcept
        : _member(fmtPair.first), _format(fmtPair.second), _name(name)
    {}

    template<typename Encoding>
    void
    FromJson(Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper&)
    {
        if (auto findItr = jsonValue.FindMember(_name); findItr != jsonValue.MemberEnd()) {
            istringstream_t in(getValue<string_t>(findItr->value));

            in >> date::parse(_format, object.*_member);
        } else {
            notFound(_name);
        }
    }

    template<typename Writer>
    void ToJson(const Class& object, Writer& writer, ReferenceMapper&)
    {
        writer.String(_name);
        writer.String(date::format(_format, object.*_member));
    }

private:
    template<typename PT = Property>
    using FormatParseType = decltype(
        date::parse(static_cast<const CharType*>(nullptr), std::declval<PT&>()),
        date::format(static_cast<const CharType*>(nullptr), std::declval<const PT&>())
        );

    static_assert(
        boost::is_detected_v<FormatParseType, Property>,
        "Property must be callable with date::parse and date::format");

    PmdType _member;
    const CharType* _format;

    const CharType* _name;
};

/// Alias template for a formatted chrono JSON property whose absence should be ignored.
template<typename Class, typename Property>
using OptionalChronoPropertyConverter = ChronoPropertyConverter<Class, Property, &ignoreNotFound<Class>>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_PROP_CONVERTER_HPP
