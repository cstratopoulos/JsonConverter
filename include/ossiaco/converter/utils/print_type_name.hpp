// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_PRINT_TYPE_NAME_HPP
#define OSSIACO_CONVERTER_UTILS_PRINT_TYPE_NAME_HPP

#include <ossiaco/converter/utils/string_convert.hpp>

#include <boost/core/demangle.hpp>
#include <boost/config.hpp>

#include <typeinfo>
#include <type_traits>

namespace Ossiaco::converter {

template<typename T>
string_t printTypeName()
{
    static_assert(std::is_class_v<T> || std::is_enum_v<T>);

    static const auto result = []() -> string_t {
        const char* name = typeid(T).name();

#ifdef BOOST_WINDOWS
        // Windows prefixes name with...
        if constexpr(std::is_class_v<T>)
            name += 6; // 'class ' (6 chars) OR
        else
            name += 5; // 'enum ' (5 chars)
#endif // BOOST_WINDOWS

        std::string demangled = boost::core::demangle(name);

        if constexpr(std::is_same_v<string_t, std::string>)
            return demangled;
        else
            return toPlatString(demangled);
    }();

    return result;
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_PRINT_TYPE_NAME_HPP
