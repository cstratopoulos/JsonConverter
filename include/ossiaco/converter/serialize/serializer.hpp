// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_SERIALIZE_SERIALIZER_HPP
#define OSSIACO_CONVERTER_SERIALIZE_SERIALIZER_HPP

#include <ossiaco/converter/utils/inconstructible.hpp>

#include <boost/mp11/tuple.hpp>
#include <boost/type_traits/is_detected.hpp>

#include <functional>
#include <utility>

namespace Ossiaco::converter {

class ReferenceMapper;

template<typename>
class JsonConverter;

template<typename Class>
class JsonSerializer : Inconstructible {
public:
    template<typename Writer>
    static void toJson(const Class&, Writer&, ReferenceMapper&, std::function<void(Writer&)>* cb);
};

namespace detail {

// Helper trait to detect if converter uses type decorator serialization
template<typename Subject, typename Writer>
using PutDecoratorType = decltype(
    JsonConverter<Subject>::putDecorator(std::declval<const Subject&>(), std::declval<Writer&>));

} // namespace detail

template<typename Class>
template<typename Writer>
void JsonSerializer<Class>::toJson(
    const Class& object, Writer& writer, ReferenceMapper& refs, std::function<void(Writer&)>* cb)
{
    static_assert(traits::jsonSupportDetected<Class> || traits::jsonPropertiesDetected<Class>);

    writer.StartObject();

    if constexpr (boost::is_detected_v<detail::PutDecoratorType, Class, Writer>)
        JsonConverter<Class>::putDecorator(object, writer);

    if (cb)
        (*cb)(writer);


    if constexpr (traits::jsonPropertiesDetected<Class> && !traits::jsonSupportDetected<Class>) {
        boost::mp11::tuple_for_each(Class::jsonProperties(), [&](auto prop) {
            prop.toJson(object, writer, refs);
        });
    } else {
        boost::mp11::tuple_for_each(JsonConverter<Class>::_properties, [&](auto prop) {
            prop.toJson(object, writer, refs);
        });
    }

    writer.EndObject();
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_SERIALIZE_SERIALIZER_HPP
