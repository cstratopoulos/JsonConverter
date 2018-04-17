// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
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

#include <cstdint>

// TODO add std::optional

namespace Ossiaco::converter {

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
        using MsTimePoint = date::sys_time<std::chrono::milliseconds>;

        writeValue(writer, MsTimePoint(object).time_since_epoch().count());
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_VOCAB_TYPE_HPP
