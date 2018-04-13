#ifndef OSSIACO_CONVERTER_CHAR_TYPES_HPP
#define OSSIACO_CONVERTER_CHAR_TYPES_HPP

#include <ossiaco/converter/config.hpp>

#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace Ossiaco::converter {

#if OSSIACO_UNICODE
constexpr bool unicodeEnabled = true;
#else
constexpr bool unicodeEnabled = false;
#endif

using CharType = std::conditional_t<unicodeEnabled, wchar_t, char>;

using string_t        = std::basic_string<CharType>;
using string_view_t   = std::basic_string_view<CharType>;
using ostringstream_t = std::basic_ostream<CharType>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CHAR_TYPES_HPP
