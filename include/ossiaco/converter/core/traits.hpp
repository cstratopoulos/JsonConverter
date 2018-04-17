// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_TRAITS_HPP
#define OSSIACO_CONVERTER_CORE_TRAITS_HPP

#include <ossiaco/converter/core/char_types.hpp>

#include <boost/type_traits/is_detected.hpp>

#include <type_traits>

namespace Ossiaco::converter {

namespace traits {

template<typename T>
constexpr bool primitiveConvertible =
    std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_constructible_v<string_t, T>;

// Json converter exposed aliases

template<typename Class>
using JsonPropertiesType = decltype(Class::jsonProperties());

template<typename Class>
constexpr bool jsonPropertiesDetected = boost::is_detected_v<JsonPropertiesType, Class>;

template<typename Class>
using JsonBaseType = typename Class::JsonBase;

template<typename Class>
constexpr bool jsonBaseDetected = boost::is_detected_v<JsonBaseType, Class>;

template<typename Class>
using JsonEnumType = typename Class::JsonEnumType;

template<typename Class>
constexpr bool jsonEnumDetected = boost::is_detected_v<JsonEnumType, Class>;

struct FinalSupportTag {};

struct PolySupportTag {};

template<typename Class>
using JsonConverterSupportTagType = typename Class::JsonConverterSupportTag;

template<typename Class>
constexpr bool jsonSupportDetected = boost::is_detected_v<JsonConverterSupportTagType, Class>;

// Traits class for access to enum type map info.
template<typename Converter>
struct ConverterMap : Default {
    using SubjectEnumType = void;
    using SubjectEnumMapType = void;
};

// Smart pointer stuff

template<typename Class>
using ElementType = typename Class::element_type;

template<typename Class>
constexpr bool elementTypeDetected = boost::is_detected_v<ElementType, Class>;


} // namespace traits

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_TRAITS_HPP
