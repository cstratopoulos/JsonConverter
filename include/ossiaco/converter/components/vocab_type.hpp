// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_VOCAB_TYPE_HPP
#define OSSIACO_CONVERTER_COMPONENTS_VOCAB_TYPE_HPP

#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/components/vocab_type_fwd.hpp>

namespace Ossiaco::converter {

/// null or convertible to `std::optional::value_type`.
template<typename Property>
template<typename Encoding>
void ConvertVocabType<std::optional<Property>>::fromJson(
    std::optional<Property>& object,
    const rapidjson::GenericValue<Encoding>& jsonValue,
    ReferenceMapper& refs)
{
    if (jsonValue.IsNull()) {
        object.reset();
    } else {
        object.emplace();
        DeducedConverter::fromJson(*object, jsonValue, refs);
    }
}

template<typename Property>
template<typename Writer>
void ConvertVocabType<std::optional<Property>>::toJson(
    const std::optional<Property>& object, Writer& writer, ReferenceMapper& refs)
{
    if (!object) {
        writer.Null();
    } else {
        DeducedConverter::toJson(*object, writer, refs);
    }
}

/// Convert a [std::chrono::duration] interpreted as `Rep` count.
///
/// For flexible formatting use [ChronoPropertyConverter].
template<typename Rep, typename Period>
template<typename Encoding>
void ConvertVocabType<std::chrono::duration<Rep, Period>>::fromJson(
    Duration& object, const rapidjson::GenericValue<Encoding>& value, ReferenceMapper&)
{
    object = Duration(getValue<Rep>(value));
}

template<typename Rep, typename Period>
template<typename Writer>
void ConvertVocabType<std::chrono::duration<Rep, Period>>::toJson(
    const Duration& object, Writer& writer, ReferenceMapper&)
{
    writeValue(writer, object.count());
}

template<typename Duration>
template<typename Encoding>
void ConvertVocabType<date::sys_time<Duration>>::fromJson(
    TimePoint& object, const rapidjson::GenericValue<Encoding>& value, ReferenceMapper&)
{
    const auto epochMs = std::chrono::milliseconds(getValue<int64_t>(value));

    object = TimePoint(std::chrono::duration_cast<Duration>(epochMs));
}

template<typename Duration>
template<typename Writer>
void ConvertVocabType<date::sys_time<Duration>>::toJson(
    const TimePoint& object, Writer& writer, ReferenceMapper&)
{
    writeValue(
        writer,
        static_cast<int64_t>(std::chrono::time_point_cast<std::chrono::milliseconds>(object)
                                 .time_since_epoch()
                                 .count()));
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_VOCAB_TYPE_HPP
