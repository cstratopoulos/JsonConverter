// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_SMART_PTR_HPP
#define OSSIACO_CONVERTER_COMPONENTS_SMART_PTR_HPP

#include <ossiaco/converter/core/exceptions.hpp>
#include <ossiaco/converter/core/reference_mapper.hpp>
#include <ossiaco/converter/core/traits.hpp>

#include <boost/type_traits/detected.hpp>
#include <rapidjson/document.h>

#include <cstdint>

namespace Ossiaco::converter {

template<typename>
class JsonConverter;

template<typename SmartPtr>
struct ConvertSmartPtr {
    using PropertyType = boost::detected_t<traits::ElementType, SmartPtr>;

    static_assert(
        traits::jsonSupportDetected<PropertyType>,
        "Smart pointer conversion only supported for user-defined types with macro-enabled "
        "conversion");

    template<typename Encoding>
    static void fromJson(SmartPtr&, const rapidjson::GenericValue<Encoding>&, ReferenceMapper&);

    template<typename Writer>
    static void toJson(const SmartPtr&, Writer&, ReferenceMapper&);
};

namespace detail {

// Obtain a raw pointer managed by a [std::shared_ptr] or [std::weak_ptr]
template<template<typename> typename PtrType, typename PtrElem>
PtrElem* getPtr(const PtrType<PtrElem>& smartPtr)
{
    using SmartPtr = PtrType<PtrElem>;

    if constexpr (std::is_same_v<SmartPtr, std::shared_ptr<PtrElem>>)
        return smartPtr.get();
    else if constexpr (std::is_same_v<SmartPtr, std::weak_ptr<PtrElem>>)
        return smartPtr.lock().get();
}

} // namespace detail

template<typename SmartPtr>
template<typename Encoding>
void ConvertSmartPtr<SmartPtr>::fromJson(
    SmartPtr& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
{
    if (auto idItr = jsonValue.FindMember(OSSIACO_XPLATSTR("@id")); idItr == jsonValue.MemberEnd()) {
        auto refItr = jsonValue.FindMember(OSSIACO_XPLATSTR("@ref"));

        if (refItr == jsonValue.MemberEnd()) {
            // If there is no id it must be a ref
            throw RefMissing<PropertyType>();
        }

        object = (refs.template get<PropertyType>((refItr->value).GetInt()));
    } else {
        auto resolver = JsonConverter<PropertyType>::template resolveTypeAllocator<PropertyType, Encoding>(jsonValue);

        object = refs.add((idItr->value).GetInt(), resolver.allocate());
        resolver.deserialize(*detail::getPtr(object), jsonValue, refs);
    }
}

template<typename SmartPtr>
template<typename Writer>
void ConvertSmartPtr<SmartPtr>::toJson(
    const SmartPtr& object, Writer& writer, ReferenceMapper& refs)
{
    const PropertyType* managedPtr = detail::getPtr(object);

    if (managedPtr) {
        auto key   = reinterpret_cast<uintptr_t>(managedPtr);
        int32_t id = refs.at(key);

        if (id) {
            writer.StartObject();
            writer.String(OSSIACO_XPLATSTR("@ref"));
            writer.Int(id);
            writer.EndObject();
        } else {
            id = refs.add(key);

            std::function<void(Writer&)> cb = [id](Writer& w) {
                w.String(OSSIACO_XPLATSTR("@id"));
                w.Int(id);
            };

            managedPtr->ToJson(writer, refs, &cb);
        }
    } else {
        writer.Null();
    }
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_SMART_PTR_HPP
