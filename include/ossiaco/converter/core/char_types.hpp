#ifndef OSSIACO_CONVERTER_CHAR_TYPES_HPP
#define OSSIACO_CONVERTER_CHAR_TYPES_HPP

#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

#ifndef OSSIACO_UNICODE
#ifdef _UNICODE
#define OSSIACO_UNICODE
#endif // _UNICODE
#endif // OSSIACO_UNICODE

#ifdef OSSIACO_UNICODE
#define OSSIACO_XPLATSTR(x) L##x
#else
#define OSSIACO_XPLATSTR(x) x
#endif

namespace Ossiaco::converter {

#ifdef OSSIACO_UNICODE
constexpr bool unicodeDefined = true;
#else
constexpr bool unicodeDefined = false;
#endif

using CharType = std::conditional_t<unicodeDefined, wchar_t, char>;

using string_t        = std::basic_string<CharType>;
using string_view_t   = std::basic_string_view<CharType>;
using ostringstream_t = std::basic_ostream<CharType>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CHAR_TYPES_HPP
