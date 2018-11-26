// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP
#define OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP

#include <ossiaco/converter/core/traits.hpp>

#include <boost/type_traits/is_detected.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view/map.hpp>

namespace Ossiaco::converter {

namespace detail {

// N.B.: range_value_t is deprecated in favor of range_value_type_t but we use this for
// compatibility with the range-v3-vs2015 fork which still has the old trait
template<typename Rng>
using RangeValueType = ranges::range_value_t<Rng>;

template<typename Rng>
void pushBackInsert(Rng& rng, RangeValueType<Rng>&& val);

} // namespace detail

template<typename>
struct ConverterDeductor;

class ReferenceMapper;

template<typename Container>
struct ConvertLinearRange {
    static_assert(ranges::InputRange<Container>::value);

    using PropertyType      = detail::RangeValueType<Container>;
    using PropTypeConverter = typename ConverterDeductor<PropertyType>::Type;

    template<typename Encoding>
    static void fromJson(
        Container& container,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& references)
    {
        for (const auto& arrayEntry : jsonValue.GetArray()) {
            PropertyType elem{};
            PropTypeConverter::fromJson(elem, arrayEntry, references);

            ranges::action::push_back(container, std::move(elem));
        }
    }

    template<typename Writer>
    static void toJson(const Container& container, Writer& writer, ReferenceMapper& references)
    {
        writer.StartArray();

        for (const PropertyType& containerElem : container)
            PropTypeConverter::toJson(containerElem, writer, references);

        writer.EndArray();
    }
};

template<typename KeyValMap>
struct ConvertKeyValueRange {
    static_assert(ranges::view::keys_fn::Concept<KeyValMap>::value);

    using ValueType           = detail::RangeValueType<KeyValMap>;
    using KeyType             = decltype(std::declval<ValueType>().first);
    using MappedType          = decltype(std::declval<ValueType>().second);
    using MappedTypeConverter = typename ConverterDeductor<MappedType>::Type;

    static_assert(
        std::is_constructible_v<string_t, KeyType>,
        "KeyValue ranges are mapped to JSON (sub-) objects so they require string-like keys");

    template<typename Encoding>
    static void fromJson(
        KeyValMap& kvMap,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& references)
    {
        for (auto iter = jsonValue.MemberBegin(); iter != jsonValue.MemberEnd(); ++iter) {
            MappedType value{};

            MappedTypeConverter::fromJson(value, iter->value, references);
            kvMap[iter->name.GetString()] = std::move(value);
        }
    }

    template<typename Writer>
    static void toJson(const KeyValMap& kvMap, Writer& writer, ReferenceMapper& references)
    {
        writer.StartObject();

        // This could be const auto& [key, val] but this way works with clang 4 too.
        for (const auto& kvPair : kvMap) {
            writer.String(kvPair.first);
            MappedTypeConverter::toJson(kvPair.second, writer, references);
        }

        writer.EndObject();
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP
