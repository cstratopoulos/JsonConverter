// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_JSON_PROPERTY_HPP
#define OSSIACO_CONVERTER_COMPONENTS_JSON_PROPERTY_HPP

#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/utils/accessors.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>

#include <boost/type_traits/remove_cv_ref.hpp>

#include <type_traits>

namespace Ossiaco::converter {

class ReferenceMapper;

enum class NullValuePolicy {
    silent, // do nothing
    write   // write a null value to the JSON
};

// Interface for JSON conversion of member data.
//
// Delegates to a deduced converter, wrapping the common logic of writing a field name or
// checking for member existence.
// Note one or both of `toJson`/`fromJson` will be present.
// When deserializing the default policy is to throw an exception if the value is not
// present, unless we're converting an std::optional in which case it will be ignored.
template<
    typename MemberPtr,
    NullValuePolicy nullValPolicy = NullValuePolicy::silent,
    NotFoundHandlerPtr notFound   = nullptr>
class JsonProperty {
public:
    static_assert(std::is_member_pointer_v<MemberPtr>);

    using Class    = ClassOf<MemberPtr>;
    using Property = PropertyOf<MemberPtr>;

    static constexpr NullValuePolicy nullValuePolicy = nullValPolicy;

    static constexpr bool enableFromJson = hasMutableAccess<MemberPtr>;
    static constexpr bool enableToJson   = hasConstAccess<MemberPtr>;

    static_assert(
        enableFromJson || enableToJson,
        "JsonProperty must allow conversion in at least one direction");

    constexpr JsonProperty(MemberPtr member, const CharType* name) noexcept
        : _member(member), _name(name)
    {}

    template<typename Encoding, typename Void = void>
    auto fromJson(
        Class& object,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& refs) const -> std::enable_if_t<enableFromJson, Void>
    {
        if (auto findItr = jsonValue.FindMember(_name); findItr != jsonValue.MemberEnd()) {
            DeducedConverter::fromJson(std::invoke(_member, object), findItr->value, refs);
        } else if constexpr (notFound != nullptr) {
            notFound(_name);
        } else if constexpr (!isSpecialization<Property, std::optional>) {
            throw RequiredPropertyMissing<Class>(_name);
        }
    }

    template<typename Writer, typename Void = void>
    auto toJson(const Class& object, Writer& writer, ReferenceMapper& refs) const
        -> std::enable_if_t<enableToJson, Void>
    {
        const auto& field = std::invoke(_member, object);

        if constexpr (
            nullValPolicy == NullValuePolicy::silent && isSpecialization<Property, std::optional>) {
            if (!field.has_value()) {
                return;
            }
        }

        writer.String(_name);
        DeducedConverter::toJson(field, writer, refs);
    }

private:
    MemberPtr _member;

    const CharType* _name;
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_JSON_PROPERTY_HPP
