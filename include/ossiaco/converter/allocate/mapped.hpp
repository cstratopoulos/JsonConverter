// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
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
#include <ossiaco/converter/utils/inconstructible.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>

#include <type_traits>

namespace Ossiaco::converter {

template<typename>
class JsonConverter;

template<typename Converter>
class MappedTypeAllocator : Inconstructible {
public:
    using ConverterProperties = traits::ConverterProperties<Converter>;

    using SubjectType       = typename ConverterProperties::SubjectType;
    using ConverterEnumType = typename ConverterProperties::SubjectEnumType;
    using ConverterEnumMap  = typename ConverterProperties::SubjectEnumMapType;

    static_assert(isCustomized<ConverterProperties>);

    template<typename Derived, typename Encoding>
    static bool registerDerivedClass()
    {
        adlInvokeEnumMappedHook<SubjectType, Derived, ConverterEnumType>();

        return Converter::template ensureRegisteredWithBase<Derived, Encoding>();
    }

    template<typename Encoding>
    static auto resolveTypeAllocator(const rapidjson::GenericValue<Encoding>& jsonValue)
    {
        return ConverterEnumMap::template resolveAllocator<SubjectType, Encoding>(jsonValue);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_MAPPED_HPP
