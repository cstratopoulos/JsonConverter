#ifndef OSSIACO_CONVERTER_STRING_CONVERT_HPP
#define OSSIACO_CONVERTER_STRING_CONVERT_HPP

#include <ossiaco/converter/core/char_types.hpp>

#include <codecvt>
#include <locale>

namespace Ossiaco::converter {

template<typename StringType>
string_t toPlatString(const StringType&);

template<typename StringType>
std::string toNarrowString(const StringType&);

template<typename StringType>
string_t toPlatString(const StringType& str)
{
    if constexpr (std::is_same_v<string_t, StringType>) {
        return str;
    } else {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

        if constexpr (std::is_same_v<StringType, std::string>) {
            // converting std::string -> std::wstring
            return conv.from_bytes(str);
        } else if constexpr (std::is_same_v<StringType, std::wstring>) {
            // converting std::wstring -> std::string
            return conv.to_bytes(str);
        }
    }
}

template<typename StringType>
std::string toNarrowString(const StringType& str)
{
    if constexpr(std::is_same_v<StringType, std::wstring>) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(str);
    } else {
        return str;
    }
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_STRING_CONVERT_HPP
