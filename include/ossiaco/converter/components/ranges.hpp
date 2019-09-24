// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP
#define OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP

#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/core/traits.hpp>

#include <boost/type_traits/is_detected.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>

namespace Ossiaco::converter {

template<typename>
struct ConvertRange;

class ReferenceMapper;

namespace detail {

template<typename Rng>
using RangeValueType = ranges::range_value_type_t<Rng>;

template<typename Rng>
constexpr bool kvRangeConcept = ranges::detail::kv_pair_like_<ranges::range_reference_t<Rng>>;

template<typename Rng>
void pushBackInsert(Rng& rng, RangeValueType<Rng>&& val);

template<typename Cont, typename Enable = void>
struct ConvertRange;

template<typename Cont>
struct ConvertRange<
    Cont,
    std::enable_if_t<ranges::input_range<Cont> && !detail::kvRangeConcept<Cont>>> {

    template<typename Encoding>
    static void fromJson(
        Cont& container,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& references)
    {
        auto const& jsonArray = jsonValue.GetArray();

        container = ranges::to<Cont>(
            ranges::views::transform(jsonArray, [&references](auto const& arrayEntry) {
                ranges::range_value_type_t<Cont> elem{};
                DeducedConverter::fromJson(elem, arrayEntry, references);
                return elem;
            }));
    }

    template<typename Writer>
    static void toJson(const Cont& container, Writer& writer, ReferenceMapper& references)
    {
        writer.StartArray();

        for (const ranges::range_value_type_t<Cont>& containerElem : container) {
            DeducedConverter::toJson(containerElem, writer, references);
        }

        writer.EndArray();
    }
};

template<typename KeyValMap>
struct ConvertRange<
    KeyValMap,
    std::enable_if_t<ranges::input_range<KeyValMap> && detail::kvRangeConcept<KeyValMap>>> {

    using ValueType  = ranges::range_value_type_t<KeyValMap>;
    using KeyType    = decltype(std::declval<ValueType>().first);
    using MappedType = decltype(std::declval<ValueType>().second);

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

            DeducedConverter::fromJson(value, iter->value, references);
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
            DeducedConverter::toJson(kvPair.second, writer, references);
        }

        writer.EndObject();
    }
};

} // namespace detail

template<typename Cont>
struct ConvertRange : detail::ConvertRange<Cont> {};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_RANGES_HPP
