// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_ACCESSORS_HPP
#define OSSIACO_CONVERTER_UTILS_ACCESSORS_HPP

#include <ossiaco/converter/config.hpp>

#include <boost/callable_traits/class_of.hpp>
#include <boost/callable_traits/is_invocable.hpp>

#include <functional>
#include <type_traits>

namespace Ossiaco::converter {

template<typename T>
using ClassOf = boost::callable_traits::class_of_t<T>;

#if OSSIACO_LIBCPP_INVOCABLE_WORKAROUND

namespace detail {

template<typename MemPtr>
auto constInvokeCheck(MemPtr m, const ClassOf<MemPtr>& c)
    -> decltype(std::invoke(m, c), std::true_type{});

template<typename MemPtr>
auto mutableInvokeCheck(MemPtr m, ClassOf<MemPtr>& c)
    -> decltype(std::invoke(m, c), std::true_type{});

auto constInvokeCheck(...) -> std::false_type;
auto mutableInvokeCheck(...) -> std::false_type;

}

template<typename MemPtr>
constexpr bool hasConstAccess =
    decltype(detail::constInvokeCheck(MemPtr{}, std::declval<const ClassOf<MemPtr>&>())){};

template<typename MemPtr>
constexpr bool hasMutableAccess =
    decltype(detail::mutableInvokeCheck(MemPtr{}, std::declval<ClassOf<MemPtr>&>())){};

#else

template<typename MemPtr>
constexpr bool hasConstAccess = std::is_invocable_v<MemPtr, const ClassOf<MemPtr>&>;

template<typename MemPtr>
constexpr bool hasMutableAccess = std::is_invocable_v<MemPtr, ClassOf<MemPtr>&>;

#endif

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_ACCESSORS_HPP
