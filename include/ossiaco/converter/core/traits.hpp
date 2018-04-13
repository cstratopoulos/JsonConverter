#ifndef OSSIACO_CONVERTER_TRAITS_HPP
#define OSSIACO_CONVERTER_TRAITS_HPP

#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/utils/is_detected.hpp>

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
constexpr bool jsonPropertiesDetected = stdxp::is_detected_v<JsonPropertiesType, Class>;

template<typename Class>
using JsonBaseType = typename Class::JsonBase;

template<typename Class>
constexpr bool jsonBaseDetected = stdxp::is_detected_v<JsonBaseType, Class>;

template<typename Class>
using JsonEnumType = typename Class::JsonEnumType;

template<typename Class>
constexpr bool jsonEnumDetected = stdxp::is_detected_v<JsonEnumType, Class>;

template<typename Class>
using JsonConverterSupportTagType = typename Class::JsonConverterSupportTag;

template<typename Class>
constexpr bool jsonSupportDetected = stdxp::is_detected_v<JsonConverterSupportTagType, Class>;

// Smart pointer stuff

template<typename Class>
using ElementType = typename Class::element_type;

template<typename Class>
constexpr bool elementTypeDetected = stdxp::is_detected_v<ElementType, Class>;


} // namespace traits

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_TRAITS_HPP
