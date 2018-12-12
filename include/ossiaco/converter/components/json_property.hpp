// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
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
#include <ossiaco/converter/core/not_found.hpp>
#include <ossiaco/converter/utils/accessors.hpp>

#include <boost/type_traits/remove_cv_ref.hpp>

#include <type_traits>

namespace Ossiaco::converter {

class ReferenceMapper;

// Interface for JSON conversion of member data.
//
// Delegates to a deduced converter, wrapping the common logic of writing a field name or 
// checking for member existence. 
// Note one or both of `toJson`/`fromJson` will be present.
template<
    typename MemberPtr,
    NotFoundHandlerPtr notFound = throwNotFound<ClassOf<MemberPtr>>>
class JsonProperty {
public:
    static_assert(std::is_member_pointer_v<MemberPtr>);

    using Class = ClassOf<MemberPtr>;

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
        Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs) const
        -> std::enable_if_t<enableFromJson, Void>
    {
        if (auto findItr = jsonValue.FindMember(_name); findItr != jsonValue.MemberEnd()) {
            auto& field = std::invoke(_member, object);
            DeducedConverterType<boost::remove_cv_ref_t<decltype(field)>>::fromJson(
                object.*_member, findItr->value, refs);
        } else {
            notFound(_name);
        }
    }

    template<typename Writer, typename Void = void>
    auto toJson(const Class& object, Writer& writer, ReferenceMapper& refs) const
        -> std::enable_if_t<enableToJson, Void>
    {
        // TODO optional handling sketch:
        // JsonProperty<MemberPtr, NotFoundHandlerPtr, NullValuePolicy = SilentNull>
        // Could also have NotFoundHandlerPtr default to ignoreNotFound for
        // std::optional, throwNotFound otherwise
        /*
            if constexpr(std::is_same_v<NullValuePolicy, SilentNull>
                    && isSpecialization<Class, std::optional>) {
                if (!field.has_value())
                        return;
            }
        */

        writer.String(_name);

        const auto& field = std::invoke(_member, object);

        DeducedConverterType<boost::remove_cv_ref_t<decltype(field)>>::toJson(field, writer, refs);
    }

private:
    MemberPtr _member;

    const CharType* _name;
};

template<typename MemberPtr>
using RequiredJsonProperty = JsonProperty<MemberPtr, throwNotFound<ClassOf<MemberPtr>>>;

template<typename MemberPtr>
using OptionalJsonProperty = JsonProperty<MemberPtr, &ignoreNotFound>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_JSON_PROPERTY_HPP
