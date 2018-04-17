// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_NOT_FOUND_HPP
#define OSSIACO_CONVERTER_CORE_NOT_FOUND_HPP

#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/exceptions.hpp>

namespace Ossiaco::converter {

template<typename Class>
void throwNotFound(string_view_t propName)
{
    throw RequiredPropertyMissing<Class>(propName);
}

template<typename>
void ignoreNotFound(string_view_t) {}

/// Function pointer as policy template parameter for handling missing JSON property.
///
/// In [PropertyConverter] and [ChronoPropertyConverter] a non-type template parameter of this
/// type can be provided to declare how missing JSON properties should be found.
/// `NotFoundHandlerPtr(propName)` is called when deserializing JSON which has no member named
/// `propName`.
template<typename Class>
using NotFoundHandlerPtr = decltype(&throwNotFound<Class>);

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_NOT_FOUND_HPP
