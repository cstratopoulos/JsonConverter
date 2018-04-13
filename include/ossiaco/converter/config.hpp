#ifndef OSSIACO_CONVERTER_CONFIG_HPP
#define OSSIACO_CONVERTER_CONFIG_HPP

#ifndef OSSIACO_UNICODE
#    ifdef _UNICODE
#        define OSSIACO_UNICODE 1
#    else
/// Controls whether unicode is enabled.
/// It is disabled by default unless the _UNICODE macro is set.
#        define OSSIACO_UNICODE 0
#    endif
#endif

#if OSSIACO_UNICODE
#    define OSSIACO_XPLATSTR(x) L##x
#else
/// Macro to wrap all instances of character or string literals for conformance with unicode setting.
#    define OSSIACO_XPLATSTR(x) x
#endif

#ifndef OSSIACO_RAPIDJSON_ASSERT_THROW
/// Controls whether RAPIDJSON_ASSERT will be redefined to enable exception throws.
/// It is disabled by default
#    define OSSIACO_RAPIDJSON_ASSERT_THROW 0
#endif 

#endif // OSSIACO_CONVERTER_CONFIG_HPP
