// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_EXCEPTIONS_HPP
#define OSSIACO_CONVERTER_CORE_EXCEPTIONS_HPP

#include <ossiaco/converter/config.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/utils/print_type_name.hpp>
#include <ossiaco/converter/utils/string_convert.hpp>

#include <rapidjson/error/error.h>

#include <stdexcept>
#include <type_traits>

#include <cstdint>

namespace Ossiaco::converter {

// Base class for exceptions that can occur in the JSON conversion process
class SerializationException;

// A required JSON property was not found when deserializing Class.
template<typename Class>
class RequiredPropertyMissing;

// Attempted @type-decorator polymorphic deserialization of a class not registered as an
// inheritor of Base
template<typename Base>
class UnregisteredType;

// Attempted to allocate abstract type Class.
template<typename Class>
class AbstractTypeAllocation;

// Expected an '@ref' element when deserializing Class
template<typename Class>
class RefMissing;

// The reference ID was not found in the JSON.
class InvalidReferenceId;

// Expected some sort of type field to map Enum to an instance of Class
template<typename Class, typename Enum>
class TypeFieldMissing;

// Wraps a file I/O error.
class OpenFileError;

// If OSSIACO_RAPIDJSON_ASSERT_THROW is defined, this class represents an occurrence of
// RAPIDJSON_ASSERT which has been turned into an an exception throw.
class RapidJsonAssert;

// An exception which wraps a rapidjson::ParseResult.
class RapidJsonParseError;

namespace detail {

// Create a space-separated string of arg, args...
template<typename Arg, typename... Args>
string_t streamFormat(const Arg& arg, const Args&... args);

}

class SerializationException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    SerializationException(const std::wstring& str) : SerializationException(toNarrowString(str)) {}
};

template<typename Class>
class RequiredPropertyMissing : public SerializationException {
public:
    RequiredPropertyMissing(string_view_t propName)
        : SerializationException(detail::streamFormat(
              OSSIACO_XPLATSTR("The required property"),
              propName,
              OSSIACO_XPLATSTR("was not found while deserializing"),
              printTypeName<Class>()))
    {}
};

template<typename Base>
class UnregisteredType : public SerializationException {
public:
    UnregisteredType(string_view_t derivedClassName)
        : SerializationException(detail::streamFormat(
              OSSIACO_XPLATSTR("Type"),
              derivedClassName,
              OSSIACO_XPLATSTR("is not registered for polymorphic deserialization from"),
              printTypeName<Base>()))
    {}

};

template<typename Class>
class AbstractTypeAllocation : public SerializationException {
    AbstractTypeAllocation()
        : SerializationException(detail::streamFormat(
              OSSIACO_XPLATSTR("Attempted to allocate abstract type"), printTypeName<Class>()))
    {}
};

template<typename Class>
class RefMissing : public SerializationException {
public:
    RefMissing()
        : SerializationException(detail::streamFormat(
              OSSIACO_XPLATSTR("No '@id' element found when deserializing smart pointer to"),
              printTypeName<Class>(),
              OSSIACO_XPLATSTR("expected '@ref'")))
    {}
};

class InvalidReferenceId : public SerializationException {
public:
    InvalidReferenceId(int32_t id)
        : SerializationException(detail::streamFormat(
            OSSIACO_XPLATSTR("The reference"), id, OSSIACO_XPLATSTR("was not found in the JSON")))
    {}
};

template<typename Class, typename Enum>
class TypeFieldMissing : public SerializationException {
public:
    static_assert(std::is_class_v<Class> && std::is_enum_v<Enum>);

    TypeFieldMissing(string_view_t fieldName)
        : SerializationException(detail::streamFormat(
              OSSIACO_XPLATSTR("Expected"),
              fieldName,
              OSSIACO_XPLATSTR("field in JSON needed to map enum"),
              printTypeName<Enum>(),
              OSSIACO_XPLATSTR("to an instance of"),
              printTypeName<Class>()))
    {}
};

class OpenFileError : public SerializationException {
public:
    OpenFileError(const CharType* fileName)
        : SerializationException(detail::streamFormat(OSSIACO_XPLATSTR("Failed to open file"), fileName))
    {}
};

#if OSSIACO_RAPIDJSON_ASSERT_THROW

class RapidJsonAssert : public SerializationException {
public:
    using SerializationException::SerializationException;
};

#endif // OSSIACO_RAPIDJSON_ASSERT_THROW

class RapidJsonParseError : public SerializationException {
public:
    RapidJsonParseError(const rapidjson::ParseResult& r)
        : SerializationException(detail::streamFormat(
            OSSIACO_XPLATSTR("RapidJSON parse error: code"), r.Code(), OSSIACO_XPLATSTR("at offset"), r.Offset()))
    {}
};

namespace detail {

template<typename Arg, typename... Args>
string_t streamFormat(const Arg& arg, const Args&... args)
{
    ostringstream_t out;

    out << arg;
    ((out << OSSIACO_XPLATSTR(' ') << args), ...);

    return out.str();
}

} // namespace detail

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_EXCEPTIONS_HPP
