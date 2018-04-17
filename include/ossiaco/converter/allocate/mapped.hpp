// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_MAPPED_HPP
#define OSSIACO_CONVERTER_ALLOCATE_MAPPED_HPP

#include <ossiaco/converter/allocate/enum_to_type.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/hooks/logging.hpp>
#include <ossiaco/converter/utils/customized.hpp>

#include <boost/type_traits/nonesuch.hpp>

#include <type_traits>

namespace Ossiaco::converter {

template<typename Converter>
class MappedTypeAllocator : boost::nonesuch {
public:
    using ConvMapTraits     = ConverterMap<Converter>;
    using ConverterEnumType = typename ConvMapTraits::SubjectEnumType;
    using ConverterEnumMap  = typename ConvMapTraits::SubjectEnumMapType;

    static_assert(isCustomized<ConvMapTraits>);

    template<typename Class, typename Derived, typename Encoding>
    static bool registerDerivedClass()
    {
        adlInvokeEnumMappedHook<Class, Derived, ConverterEnumType>();

        return Converter::template ensureRegisteredWithBase<Derived, Encoding>();
    }

    template<typename Class, typename Encoding>
    static auto resolveTypeAllocator(const rapidjson::GenericValue<Encoding>& jsonValue)
    {
        return ConverterEnumMap::template resolveAllocator<Class, Encoding>(jsonValue);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_MAPPED_HPP
