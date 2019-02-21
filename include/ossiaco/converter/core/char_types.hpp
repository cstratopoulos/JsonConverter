// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_CHAR_TYPES_HPP
#define OSSIACO_CONVERTER_CORE_CHAR_TYPES_HPP

#include <ossiaco/converter/config.hpp>

#include <rapidjson/encodings.h>

#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace Ossiaco::converter {

#if OSSIACO_WCHAR_UNICODE
constexpr bool wcharUnicodeEnabled = true;
#else
constexpr bool wcharUnicodeEnabled = false;
#endif

using CharType = std::conditional_t<wcharUnicodeEnabled, wchar_t, char>;

using string_t        = std::basic_string<CharType>;
using string_view_t   = std::basic_string_view<CharType>;
using ostringstream_t = std::basic_ostringstream<CharType>;
using istringstream_t = std::basic_istringstream<CharType>;

using utf_t = std::conditional_t<wcharUnicodeEnabled, rapidjson::UTF16LE<>, rapidjson::UTF8<>>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_CHAR_TYPES_HPP
