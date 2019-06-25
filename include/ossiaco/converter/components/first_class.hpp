// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP
#define OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP

#include <ossiaco/converter/components/template_get_write.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/serialize/deserializer.hpp>
#include <ossiaco/converter/serialize/serializer.hpp>
#include <ossiaco/converter/utils/select_overload.hpp>

#include <boost/hof/returns.hpp>
#include <rapidjson/document.h>

#include <type_traits>

namespace Ossiaco::converter {

class ReferenceMapper;

class ConvertFirstClass {
public:
    template<typename C, typename Encoding, typename Cfc = ConvertFirstClass>
    static auto
    fromJson(C& object, const rapidjson::GenericValue<Encoding>& jv, ReferenceMapper& refs)
        BOOST_HOF_RETURNS(Cfc::fromJson(object, jv, refs, selectOverload));

    template<typename C, typename Writer, typename Cfc = ConvertFirstClass>
    static auto toJson(const C& object, Writer& writer, ReferenceMapper& refs)
        BOOST_HOF_RETURNS(Cfc::toJson(object, writer, refs, selectOverload));

private:
    template<typename C, typename Encoding>
    static std::enable_if_t<traits::jsonPropertiesDetected<C>> fromJson(
        C& object,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& refs,
        OverloadRank<0>)
    {
        JsonDeserializer<C, Encoding>::fromJson(object, jsonValue, refs);
    }

    template<typename C, typename Encoding>
    static auto fromJson(
        C& object,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper&,
        OverloadRank<1>) BOOST_HOF_RETURNS(static_cast<void>(object = C(jsonValue)));

    template<typename C, typename Encoding>
    static auto fromJson(
        C& object,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper&,
        OverloadRank<2>) BOOST_HOF_RETURNS(static_cast<void>(object = getValue<C>(jsonValue)));

    template<typename C, typename Writer>
    static std::enable_if_t<traits::jsonPropertiesDetected<C>>
    toJson(const C& object, Writer& writer, ReferenceMapper& refs, OverloadRank<0>)
    {
        JsonSerializer<C>::template toJson<Writer>(object, writer, refs, nullptr);
    }

    template<typename C, typename Writer>
    static auto toJson(const C& object, Writer& writer, ReferenceMapper&, OverloadRank<1>)
        BOOST_HOF_RETURNS(object.toJson(writer));

    template<typename C, typename Writer>
    static auto toJson(const C& object, Writer& writer, ReferenceMapper&, OverloadRank<2>)
        BOOST_HOF_RETURNS(static_cast<void>(writeValue(writer, object)));
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_FIRST_CLASS_HPP
