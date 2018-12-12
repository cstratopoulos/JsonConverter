// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_JSON_PROPERTY_HPP
#define OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_JSON_PROPERTY_HPP

#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/not_found.hpp>
#include <ossiaco/converter/utils/accessors.hpp>

#include <date/date.h>
#include <rapidjson/document.h>

#include <utility>

namespace Ossiaco::converter {

class ReferenceMapper;

// Syntactic "salt" alias which explicitly associates a chrono PMD to its format string.
template<typename MemberPtr>
using ChronoFmtPair = std::pair<MemberPtr, const CharType*>;

// Type deduction helper since there's no alias template CTAD.
template<typename MemberPtr>
constexpr auto chronoFmtPair(MemberPtr member, const CharType* format)
{
    return std::make_pair(member, format);
}

// Like JsonProperty but for a date/chrono property with formatting governed by a format string.
//
// Property must be any chrono-related type which can be streamed with the to_stream/from_stream API
// of the date library. Conversion uses date::parse/date::format, potentially enabling ADL for
// user-defined date types.
template<typename MemberPtr, NotFoundHandlerPtr notFound = throwNotFound<ClassOf<MemberPtr>>>
class ChronoJsonProperty {
public:
    static_assert(std::is_member_pointer_v<MemberPtr>);

    using Class = ClassOf<MemberPtr>;

	static constexpr bool enableFromJson = hasMutableAccess<MemberPtr>;
	static constexpr bool enableToJson   = hasConstAccess<MemberPtr>;

    static_assert(
        enableFromJson || enableToJson,
        "JsonProperty must allow conversion in at least one direction");

    constexpr ChronoJsonProperty(ChronoFmtPair<MemberPtr> fmtPair, const CharType* name) noexcept
        : _member(fmtPair.first), _format(fmtPair.second), _name(name)
    {}

    template<typename Encoding, typename Void = void>
    auto
    fromJson(Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper&) const
        -> std::enable_if_t<enableFromJson, Void>
    {
        if (auto findItr = jsonValue.FindMember(_name); findItr != jsonValue.MemberEnd()) {
            istringstream_t in(getValue<string_t>(findItr->value));

            in >> date::parse(_format, std::invoke(_member, object));
        } else {
            notFound(_name);
        }
    }

    template<typename Writer, typename Void = void>
    auto toJson(const Class& object, Writer& writer, ReferenceMapper&) const
        -> std::enable_if_t<enableToJson, Void>
    {
        writer.String(_name);
        writer.String(date::format(_format, std::invoke(_member, object)));
    }

private:
    MemberPtr _member;

    const CharType* _format;

    const CharType* _name;
};

template<typename MemberPtr>
using RequiredChronoJsonProperty = ChronoJsonProperty<MemberPtr, throwNotFound<ClassOf<MemberPtr>>>;

template<typename MemberPtr>
using OptionalChronoJsonProperty = ChronoJsonProperty<MemberPtr, &ignoreNotFound>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_CHRONO_FMT_JSON_PROPERTY_HPP
