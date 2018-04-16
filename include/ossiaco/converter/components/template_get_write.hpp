// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_TEMPLATE_GET_WRITE_HPP
#define OSSIACO_CONVERTER_COMPONENTS_TEMPLATE_GET_WRITE_HPP

#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/traits.hpp>

#include <boost/mp11/utility.hpp>
#include <rapidjson/document.h>

#include <type_traits>

namespace Ossiaco::converter {

/// An interface like [std::get] for retrieving values from JSON.
///
/// \requires `Value` is [traits::primitiveConvertible].
template<typename Value, typename Encoding>
Value getValue(const rapidjson::GenericValue<Encoding>&);

/// A template-based interface for putting a value in a writer.
///
/// \requires `Value` is [traits::primitiveConvertible] and `Writer` is a `rapidjson::Writer`.
template<typename Value, typename Writer>
bool writeValue(Writer&, const Value&);

namespace detail {

// Workaround: std::underlying_type causes hard error if T is not an enum
template<typename T>
using SafeUnderlyingType =
    boost::mp11::mp_eval_if_c<!std::is_enum_v<T>, T, std::underlying_type_t, T>;

} // namespace detail

template<typename Value, typename Encoding>
Value getValue(const rapidjson::GenericValue<Encoding>& value)
{
    static_assert(traits::primitiveConvertible<Value>);

    if constexpr (std::is_enum_v<Value>) {
        return static_cast<Value>(getValue<detail::SafeUnderlyingType<Value>>(value));
    }

    if constexpr (std::is_same_v<Value, string_t>) {
        return value.GetString();
    }
    else if constexpr (std::is_same_v<Value, bool>) {
        return value.GetBool();
    }
    else if constexpr (std::is_same_v<Value, int32_t>) {
        return value.GetInt();
    }
    else if constexpr (std::is_same_v<Value, uint32_t>) {
        return value.GetUint();
    }
    else if constexpr (std::is_same_v<Value, int64_t>) {
        return value.GetInt64();
    }
    else if constexpr (std::is_same_v<Value, uint64_t>) {
        return value.GetUint64();
    }
    else if constexpr (std::is_same_v<Value, float>) {
        return value.GetFloat();
    }
    else if constexpr (std::is_same_v<Value, double>) {
        return value.GetDouble();
    }
    else {
        return static_cast<Value>(value.GetInt());
    }
}

template<typename Value, typename Writer>
bool writeValue(Writer& writer, const Value& value)
{
    static_assert(traits::primitiveConvertible<Value>);

    if constexpr (std::is_enum_v<Value>) {
        return writeValue(writer, static_cast<detail::SafeUnderlyingType<Value>>(value));
    }

    if constexpr (std::is_same_v<Value, string_t>) {
        return writer.String(value.c_str());
    }
    else if constexpr (std::is_same_v<Value, bool>) {
        return writer.Bool(value);
    }
    else if constexpr (std::is_same_v<Value, int32_t>) {
        return writer.Int(value);
    }
    else if constexpr (std::is_same_v<Value, uint32_t>) {
        return writer.Uint(value);
    }
    else if constexpr (std::is_same_v<Value, int64_t>) {
        return writer.Int64(value);
    }
    else if constexpr (std::is_same_v<Value, uint64_t>) {
        return writer.Uint64(value);
    }
    else if constexpr (std::is_same_v<Value, float>) {
        return writer.Float(value);
    }
    else if constexpr (std::is_same_v<Value, double>) {
        return writer.Double(value);
    }
    else {
        writer.Uint64(static_cast<uint64_t>(value));
    }
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_TEMPLATE_GET_WRITE_HPP
