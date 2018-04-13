// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CONFIG_HPP
#define OSSIACO_CONVERTER_CONFIG_HPP

#ifndef OSSIACO_WCHAR_UNICODE
/// Wether to use unicode through `char_t`. Off by default.
///
/// This should be defined for Windows projects in situations where `UNICODE`/`_UNICODE` would
/// have been by the Visual Studio properties manager.
#    define OSSIACO_WCHAR_UNICODE 0
#endif

#if OSSIACO_WCHAR_UNICODE
#    define OSSIACO_XPLATSTR(x) L##x
#else
/// Use to wrap all instances of character or string literals for conformance with unicode setting.
#    define OSSIACO_XPLATSTR(x) x
#endif

#if OSSIACO_WCHAR_UNICODE && defined(BOOST_WINDOWS)
#    define OSSIACO_XPLAT_FOPEN_S _wfopen_s
#else
/// fopen_s-style call approppriate for unicode setting
#    define OSSIACO_XPLAT_FOPEN_S fopen_s
#endif

#ifndef OSSIACO_RAPIDJSON_ASSERT_THROW
/// Controls whether RAPIDJSON_ASSERT will be redefined to enable exception throws.
/// It is disabled by default
#    define OSSIACO_RAPIDJSON_ASSERT_THROW 0
#endif 

#endif // OSSIACO_CONVERTER_CONFIG_HPP
