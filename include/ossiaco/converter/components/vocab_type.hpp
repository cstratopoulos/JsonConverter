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

#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/utils/customized.hpp>

#include <date/date.h>
#include <rapidjson/document.h>

#include <chrono>
#include <optional>

#include <ciso646>
#include <cstdint>

namespace Ossiaco::converter {

template<typename>
struct ConverterDeductor;

template<typename>
class JsonConverter;

class ReferenceMapper;

template<typename Class>
struct ConvertVocabType : Default {
    template<typename Encoding>
    static void
    fromJson(Class&, const rapidjson::GenericValue<Encoding>&, ReferenceMapper&) = delete;

    template<typename Writer>
    static void toJson(const Class&, Writer&, ReferenceMapper&) = delete;
};

/// Convert a [std::optional], which is serialized to/from a null JSON value if empty.
///
/// Note an [std::optional] property is NOT the same as making a property optional through
/// [PropertyConverter]. If `"field"` is an [std::optional] JSON property then it must be
/// deserialized from a JSON object which contains the member `"field"` whose value is either
/// null or convertible to `std::optional::value_type`.
template<typename Property>
struct ConvertVocabType<std::optional<Property>> {
    using PropertyConverter = typename ConverterDeductor<Property>::Type;

    template<typename Encoding>
    static void fromJson(std::optional<Property>& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
    {
        if (jsonValue.IsNull()) {
            object.reset();
        } else {
            object.emplace();
            PropertyConverter::fromJson(*object, jsonValue, refs);
        }
    }

    template<typename Writer>
    static void toJson(const std::optional<Property>& object, Writer& writer, ReferenceMapper& refs)
    {
        if (!object) {
            writer.Null();
        } else {
            PropertyConverter::toJson(*object, writer, refs);
        }
    }
};

/// Convert a [std::chrono::duration] interpreted as `Rep` count.
///
/// For flexible formatting use [ChronoPropertyConverter].
template<typename Rep, typename Period>
struct ConvertVocabType<std::chrono::duration<Rep, Period>> {
    using Duration = std::chrono::duration<Rep, Period>;

    template<typename Encoding>
    static void
    fromJson(Duration& object, const rapidjson::GenericValue<Encoding>& value, ReferenceMapper&)
    {
        object = Duration(getValue<Rep>(value));
    }

    template<typename Writer>
    static void toJson(const Duration& object, Writer& writer, ReferenceMapper&)
    {
        writeValue(writer, object.count());
    }
};

/// Convert a [date::sys_time] interpreted as UNIX time milliseconds.
///
/// For other time_point types, and flexible formatting, use [ChronoPropertyConverter].
template<typename Duration>
struct ConvertVocabType<date::sys_time<Duration>> {
    using TimePoint = date::sys_time<Duration>;

    template<typename Encoding>
    static void
    fromJson(TimePoint& object, const rapidjson::GenericValue<Encoding>& value, ReferenceMapper&)
    {
        const auto epochMs = std::chrono::milliseconds(getValue<int64_t>(value));

        object = TimePoint(std::chrono::duration_cast<Duration>(epochMs));
    }

    template<typename Writer>
    static void toJson(const TimePoint& object, Writer& writer, ReferenceMapper&)
    {
        writeValue(
            writer,
            static_cast<int64_t>(std::chrono::time_point_cast<std::chrono::milliseconds>(object)
                                     .time_since_epoch()
                                     .count()));
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_VOCAB_TYPE_HPP
