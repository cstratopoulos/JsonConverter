// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
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

#include <boost/hana/concat.hpp>
#include <boost/hana/size.hpp>
#include <boost/type_traits/detected.hpp>
#include <boost/type_traits/detected_or.hpp>
#include <boost/type_traits/is_detected_exact.hpp>

#include <type_traits>

namespace Ossiaco::converter {

template<typename>
class JsonConverter;

namespace detail {

template<typename Class, typename Tag>
using TagEnable = std::enable_if_t<traits::isExpectedJsonSupportTag<Class, Tag>>;

template<typename Class, typename Enable>
class JsonConverter {};

template<typename Class>
class JsonConverter<Class, TagEnable<Class, traits::FinalSupportTag>>
    : public SimpleTypeAllocator<Class> {
public:
    using SubjectType = Class;

private:
    static_assert(
        boost::hana::size(jsonProperties<Class>()),
        "JSON properties for a final supported class must be nonempty tuple");
};

template<typename Class>
class JsonConverter<Class, TagEnable<Class, traits::PolySupportTag>>
    : public PolyAllocBackend<Class> {
public:
    using SubjectType     = Class;
    using SubjectBaseType = boost::detected_or_t<void, traits::JsonBaseType, Class>;
    using SubjectEnumType = boost::detected_or_t<void, traits::JsonEnumType, Class>;

    static constexpr bool _isPolyRoot = std::is_void_v<SubjectBaseType>;

    using BaseConverter = std::conditional_t<_isPolyRoot, void, JsonConverter<SubjectBaseType>>;

    template<typename Derived = Class, typename Encoding = utf_t>
    static bool ensureRegisteredWithBase()
    {
        if constexpr (_isPolyRoot) {
            return true;
        } else {
            return JsonConverter<
                typename Class::JsonBase>::template registerDerivedClass<Derived, Encoding>();
        }
    }

private:
    static_assert(
        std::is_abstract_v<Class> || boost::hana::size(jsonProperties<Class>())(),
        "JSON properties for a non-abstract class must be a nonempty tuple either directly or "
        "inherited");
};

} // namespace detail

template<typename Class>
class JsonConverter : public detail::JsonConverter<Class> {
public:
    static constexpr bool isPolymorphic()
    {
        return std::is_same_v<typename Class::JsonConverterSupportTag, traits::PolySupportTag>;
    }
};

namespace traits {

template<typename Class>
struct ConverterProperties<detail::JsonConverter<Class, void>> {
    using SubjectType     = Class;
    using SubjectBaseType = boost::detected_or_t<void, traits::JsonBaseType, Class>;
    using SubjectEnumType = boost::detected_or_t<void, traits::JsonEnumType, Class>;

    using SubjectEnumMapType = boost::mp11::
        mp_eval_if_c<std::is_void_v<SubjectEnumType>, void, EnumTypeMap, SubjectEnumType>;
};

template<typename Class>
struct ConverterProperties<JsonConverter<Class>>
    : ConverterProperties<detail::JsonConverter<Class, void>> {};

} // namespace traits

template<typename Class>
bool jsonPolyImpl()
{
    static_assert(
        JsonConverter<Class>::isPolymorphic(),
        "Polymorphic registration is only valid for classes that support polymorphic JSON "
        "conversion");

    return JsonConverter<Class>::ensureRegisteredWithBase();
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_SERIALIZE_JSON_CONVERTER_HPP
