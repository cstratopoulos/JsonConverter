// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_DETECT_SPECIALIZATION_HPP
#define OSSIACO_CONVERTER_UTILS_DETECT_SPECIALIZATION_HPP

#include <type_traits>

// The type trait in this header is due to R. Martinho Fernandes
// see https://bitbucket.org/martinhofernandes/wheels/src/default/include/wheels/meta/type_traits.h%2B%2B?fileviewer=file-view-default#type_traits.h%2B%2B-163

namespace Ossiaco::converter {

template<typename T, template<typename...> typename Template>
struct SpecializationType : std::false_type {};

template<template<typename...> typename Template, typename... Args>
struct SpecializationType<Template<Args...>, Template> : std::true_type {};

template<typename T, template<typename...> typename Template>
constexpr bool isSpecialization = SpecializationType<T, Template>::value;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_DETECT_SPECIALIZATION_HPP
