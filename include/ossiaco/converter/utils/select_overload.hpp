// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_SELECT_OVERLOAD_HPP
#define OSSIACO_CONVERTER_UTILS_SELECT_OVERLOAD_HPP

namespace Ossiaco::converter {

template<unsigned I>
struct OverloadRank : OverloadRank<I + 1> {};

template<>
struct OverloadRank<5> {};

constexpr OverloadRank<0> selectOverload{};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_SELECT_OVERLOAD_HPP
