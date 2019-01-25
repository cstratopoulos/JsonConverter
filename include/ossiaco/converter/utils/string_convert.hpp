// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_STRING_CONVERT_HPP
#define OSSIACO_CONVERTER_UTILS_STRING_CONVERT_HPP

#include <ossiaco/converter/core/char_types.hpp>

#include <codecvt>
#include <locale>

namespace Ossiaco::converter {

template<typename StringType>
string_t toPlatString(StringType&&);

template<typename StringType>
std::string toNarrowString(StringType&&);

template<typename StringType>
string_t toPlatString(StringType&& s)
{
    if constexpr(std::is_constructible_v<string_t, StringType>) {
        return std::forward<StringType>(s);
    } else {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

        if constexpr(std::is_constructible_v<std::string, StringType>) {
            // std::string -> std::wstring
            return conv.from_bytes(std::forward<StringType>(s));
        } else {
            // std::wstring -> std::string
            return conv.to_bytes(std::forward<StringType>(s));
        }
    }
}

template<typename StringType>
std::string toNarrowString(StringType&& str)
{
    if constexpr(std::is_constructible_v<std::wstring, StringType>) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(std::forward<StringType>(str));
    } else {
        return std::forward<StringType>(str);
    }
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_STRING_CONVERT_HPP
