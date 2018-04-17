// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_CUSTOM_VALUE_HPP
#define OSSIACO_CONVERTER_COMPONENTS_CUSTOM_VALUE_HPP

#include <ossiaco/converter/utils/customized.hpp>

#include <rapidjson/document.h>

namespace Ossiaco::converter {

class ReferenceMapper;

/// Customization point for converting unmodifiable classes or overriding [ConvertVocabType].
///
/// This class should be specialized by users wishing to convert unmodifiable library code (e.g., Boost)
/// or to override a default conversion provided for vocab types, ranges, smart pointers, etc. 
/// Specializations of [ConvertCustomValue] are second in priority only to [ConvertFirstClass] types. 
template<typename Class>
struct ConvertCustomValue : Default {
    template<typename Encoding>
    static void	FromJson(Class&, const rapidjson::GenericValue<Encoding>&, ReferenceMapper&) = delete;

    template<typename Writer>
    static void ToJson(const Class&, Writer&, ReferenceMapper&) = delete;
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_CUSTOM_VALUE_HPP
