// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_SERIALIZE_PRETTY_CONVENIENT_HPP
#define OSSIACO_CONVERTER_SERIALIZE_PRETTY_CONVENIENT_HPP

#include <ossiaco/converter/core/buffers_streams_writers.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/reference_mapper.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/utils/file_handle.hpp>

#include <rapidjson/filewritestream.h>

#include <array>

// Convenience functions for serialization with some variation of rapidjson PrettyWriter.

namespace Ossiaco::converter {

/// Return a string obtained from serializing `obj` with pretty formatting/indentation.
template<typename Class>
string_t toJsonStringPretty(const Class& obj);

/// Serialize `obj` to the file at `fileName` with pretty formatting/indentation.
template<typename Class>
void toJsonFilePretty(const Class& obj, const CharType* fileName);

template<typename Class>
string_t toJsonStringPretty(const Class& obj)
{
    EncodedStringBuffer buf;
    PrettyStringBufferWriter writer(buf);

    if constexpr (traits::jsonSupportDetected<Class>) {
        obj.toJson(writer);
    } else {
        ReferenceMapper refs;
        JsonSerializer<Class>::template toJson<PrettyStringBufferWriter>(obj, writer, refs, nullptr);
    }

    return string_t(buf.GetString());
}

template<typename Class>
void toJsonFilePretty(const Class& obj, const CharType* fileName)
{
    UniqueFileHandle file(uniqueHandleFOpen(fileName, OSSIACO_XPLATSTR("w+b")));
    std::array<char, 65536> buf;

    rapidjson::FileWriteStream fs(file.get(), buf.data(), buf.size());
    EncodedFileStream eos(fs, true /* put BOM */);
    PrettyFileStreamWriter writer(eos);

    if constexpr (traits::jsonSupportDetected<Class>) {
        obj.toJson(writer);
    } else {
        ReferenceMapper refs;

        JsonSerializer<Class>::template toJson<PrettyFileStreamWriter>(obj, writer, refs, nullptr);
    }
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_SERIALIZE_PRETTY_CONVENIENT_HPP
