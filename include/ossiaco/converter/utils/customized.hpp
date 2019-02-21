// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_CUSTOMIZED_HPP
#define OSSIACO_CONVERTER_UTILS_CUSTOMIZED_HPP

#include <type_traits>

namespace Ossiaco::converter {

/// Base class for detecting customization points.
struct Default {};

/// Type trait for detecting if a customization point has been used.
template<typename CustomizationPoint>
using Customized = std::negation<std::is_base_of<Default, CustomizationPoint>>;

/// Helper variable template for [Customized].
template<typename CustomizationPoint>
constexpr bool isCustomized = Customized<CustomizationPoint>::value;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_CUSTOMIZED_HPP
