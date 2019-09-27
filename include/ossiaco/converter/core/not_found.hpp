// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
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

inline void ignoreNotFound(string_view_t) {}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_NOT_FOUND_HPP
