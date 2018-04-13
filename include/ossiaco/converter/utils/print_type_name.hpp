#ifndef OSSIACO_CONVERTER_PRINT_TYPE_NAME_HPP
#define OSSIACO_CONVERTER_PRINT_TYPE_NAME_HPP

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

        if constexpr(std::is_same_v<string_t, std::string>)
            return name;
        else
            return toPlatString(std::string(name));
    }();

    return result;
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_PRINT_TYPE_NAME_HPP
