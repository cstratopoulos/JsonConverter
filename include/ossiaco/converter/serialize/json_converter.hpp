// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_SERIALIZE_JSON_CONVERTER_HPP
#define OSSIACO_CONVERTER_SERIALIZE_JSON_CONVERTER_HPP

#include <ossiaco/converter/allocate/dispatch.hpp>
#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/components/first_class.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>

#include <boost/type_traits/detected.hpp>
#include <boost/type_traits/detected_or.hpp>

#include <type_traits>

namespace Ossiaco::converter {

/// Management of JSON serialization/deserialization for `Class`.
// TODO possibly refactor this into frontends based on final vs poly support to replace some if constexpr
// and detection idiom with tag dispatch
template<typename Class>
class JsonConverter : public DeducedAllocBackend<Class> {
public:
    using SubjectType     = Class;
    using SubjectBaseType = boost::detected_or_t<void, traits::JsonBaseType, Class>;
    using SubjectEnumType = boost::detected_or_t<void, traits::JsonEnumType, Class>;

    static constexpr bool _isPolymorphic = !std::is_void_v<SubjectBaseType>;

    using BaseConverter = std::conditional_t<_isPolymorphic, JsonConverter<SubjectBaseType>, void>;

    template<typename Derived = Class, typename Encoding = utf_t>
    static bool ensureRegisteredWithBase();

    static constexpr auto propertiesTuple()
    {
        if constexpr(_isPolymorphic) {
            return std::tuple_cat(BaseConverter::_properties, Class::jsonProperties());
        } else {
            return Class::jsonProperties();
        }
    }

    static constexpr auto _properties{propertiesTuple()};

private:
    using PropsType = boost::detected_t<traits::JsonPropertiesType, Class>;

    static_assert(traits::jsonSupportDetected<Class>);

    static_assert(isSpecialization<PropsType, std::tuple>);

    static_assert(
        std::tuple_size_v<decltype(_properties)> || std::is_abstract_v<Class>,
        "JSON properties for a non-abstract class must be a nonempty tuple provided either directly or inherited");
};

namespace traits {

template<typename Class>
struct ConverterProperties<JsonConverter<Class>> {
    using SubjectType     = Class;
    using SubjectBaseType = boost::detected_or_t<void, traits::JsonBaseType, Class>;
    using SubjectEnumType = boost::detected_or_t<void, traits::JsonEnumType, Class>;

    using SubjectEnumMapType = boost::mp11::mp_eval_if_c<
        std::is_void_v<SubjectEnumType>,
        void,
        EnumTypeMap, SubjectEnumType
    >;
};

} // namespace traits

template<typename Class>
template<typename Derived, typename Encoding>
bool JsonConverter<Class>::ensureRegisteredWithBase()
{
    if constexpr(_isPolymorphic) {
        using RegistrarSubject = typename Class::JsonBase;

        return JsonConverter<RegistrarSubject>::template registerDerivedClass<Derived, Encoding>();
    } else {
        return true;
    }
}

template<typename Class>
bool jsonPolyImpl()
{
    return JsonConverter<Class>::ensureRegisteredWithBase();
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_SERIALIZE_JSON_CONVERTER_HPP
