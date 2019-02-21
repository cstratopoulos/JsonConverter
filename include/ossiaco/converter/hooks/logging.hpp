// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_HOOKS_LOGGING_HPP
#define OSSIACO_CONVERTER_HOOKS_LOGGING_HPP

#include <boost/type_traits/is_detected.hpp>

#include <type_traits>

namespace Ossiaco::converter {

namespace detail {

template<typename Base, typename Derived>
using ConverterDecoratorHook = decltype(
    hookOssiacoConverterDecoratorLog(static_cast<Base*>(nullptr), static_cast<Derived*>(nullptr))
    );

template<typename Base, typename Derived, typename Enum>
using ConverterEnumMappedHook = decltype(hookOssiacoConverterEnumMappedLog(
    static_cast<Base*>(nullptr), static_cast<Derived*>(nullptr), static_cast<Enum*>(nullptr))
    );

} // namespace detail

template<typename Base, typename Derived>
constexpr bool decoratorHookDetected =
    boost::is_detected_v<detail::ConverterDecoratorHook, Base, Derived>;

template<typename Base, typename Derived, typename Enum>
constexpr bool enumMappedHookDetected =
    boost::is_detected_v<detail::ConverterEnumMappedHook, Base, Derived, Enum>;

template<typename Base, typename Derived>
void adlInvokeDecoratorHook()
{
    static_assert(std::is_base_of_v<Base, Derived>);

    if constexpr(decoratorHookDetected<Base, Derived>)
        hookOssiacoConverterDecoratorLog(static_cast<Base*>(nullptr), static_cast<Derived*>(nullptr));
}

template<typename Base, typename Derived, typename Enum>
void adlInvokeEnumMappedHook()
{
    static_assert(std::is_base_of_v<Base, Derived> && std::is_enum_v<Enum>);

    if constexpr(enumMappedHookDetected<Base, Derived, Enum>)
        hookOssiacoConverterEnumMappedLog(
            static_cast<Base*>(nullptr),
            static_cast<Derived*>(nullptr),
            static_cast<Enum*>(nullptr));
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_HOOKS_LOGGING_HPP
