// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP
#define OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP

#include <ossiaco/converter/components/custom_value.hpp>
#include <ossiaco/converter/components/first_class.hpp>
#include <ossiaco/converter/components/vocab_type_fwd.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/utils/customized.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>
#include <ossiaco/converter/utils/select_overload.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/map.hpp>

#include <memory>
#include <type_traits>

namespace Ossiaco::converter {

template<typename>
struct ConvertRange;

template<typename>
struct ConvertSmartPtr;

class ReferenceMapper;

class DeducedConverter {
public:
    template<typename C, typename Encoding>
    static void fromJson(C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs)
    {
        DeducedConverter::fromJson(c, jv, refs, selectOverload);
    }

    template<typename C, typename Writer>
    static void toJson(const C& c, Writer& w, ReferenceMapper& refs)
    {
        DeducedConverter::toJson(c, w, refs, selectOverload);
    }

private:
    template<typename C>
    static constexpr bool deducePointer =
        isSpecialization<C, std::shared_ptr> || isSpecialization<C, std::weak_ptr>;

    template<typename C, typename Encoding>
    static auto fromJson(
        C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs, OverloadRank<0>)
        BOOST_HOF_RETURNS(ConvertFirstClass::fromJson(c, jv, refs));

    template<typename C, typename Encoding>
    static std::enable_if_t<isCustomized<ConvertCustomValue<C>>> fromJson(
        C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs, OverloadRank<1>)
    {
        ConvertCustomValue<C>::fromJson(c, jv, refs);
    }

    template<typename C, typename Encoding>
    static auto fromJson(
        C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs, OverloadRank<2>)
        BOOST_HOF_RETURNS(ConvertVocabType<C>::fromJson(c, jv, refs));

    template<typename C, typename Encoding>
    static std::enable_if_t<ranges::InputRange<C>::value> fromJson(
        C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs, OverloadRank<3>)
    {
        ConvertRange<C>::fromJson(c, jv, refs);
    }

    template<typename C, typename Encoding>
    static std::enable_if_t<deducePointer<C>> fromJson(
        C& c, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs, OverloadRank<4>)
    {
        ConvertSmartPtr<C>::fromJson(c, jv, refs);
    }

    template<typename C, typename Writer>
    static auto toJson(const C& c, Writer& w, ReferenceMapper& refs, OverloadRank<0>)
        BOOST_HOF_RETURNS(ConvertFirstClass::toJson(c, w, refs));

    template<typename C, typename Writer>
    static std::enable_if_t<isCustomized<ConvertCustomValue<C>>>
    toJson(const C& c, Writer& w, ReferenceMapper& refs, OverloadRank<1>)
    {
        ConvertCustomValue<C>::toJson(c, w, refs);
    }

    template<typename C, typename Writer>
    static auto toJson(const C& c, Writer& w, ReferenceMapper& refs, OverloadRank<2>)
        BOOST_HOF_RETURNS(ConvertVocabType<C>::toJson(c, w, refs));

    template<typename C, typename Writer>
    static std::enable_if_t<ranges::InputRange<C>::value>
    toJson(const C& c, Writer& w, ReferenceMapper& refs, OverloadRank<3>)
    {
        ConvertRange<C>::toJson(c, w, refs);
    }

    template<typename C, typename Writer>
    static std::enable_if_t<deducePointer<C>>
    toJson(const C& c, Writer& w, ReferenceMapper& refs, OverloadRank<4>)
    {
        ConvertSmartPtr<C>::toJson(c, w, refs);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP
