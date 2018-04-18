// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_BUFFERS_STREAMS_WRITERS_HPP
#define OSSIACO_CONVERTER_CORE_BUFFERS_STREAMS_WRITERS_HPP

#include <ossiaco/converter/core/char_types.hpp>

#include <rapidjson/encodedstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace Ossiaco::converter {

using DefaultStringBuffer = rapidjson::GenericStringBuffer<utf_t>;
using DefaultStringWriter = rapidjson::Writer<DefaultStringBuffer, utf_t, utf_t>;

using EncodedFileStream         = rapidjson::EncodedOutputStream<utf_t, rapidjson::FileWriteStream>;
using EncodedStringBuffer       = DefaultStringBuffer;
using EncodedInsituStringStream = rapidjson::GenericInsituStringStream<utf_t>;

using FileStreamWriter         = rapidjson::Writer<EncodedFileStream, utf_t, utf_t>;
using StringBufferWriter       = rapidjson::Writer<EncodedStringBuffer, utf_t, utf_t>;
using InsituStringStreamWriter = rapidjson::Writer<EncodedInsituStringStream, utf_t, utf_t>;

using PrettyFileStreamWriter         = rapidjson::PrettyWriter<EncodedFileStream, utf_t, utf_t>;
using PrettyStringBufferWriter       = rapidjson::PrettyWriter<EncodedStringBuffer, utf_t, utf_t>;
using PrettyInsituStringStreamWriter = rapidjson::PrettyWriter<EncodedInsituStringStream, utf_t, utf_t>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_BUFFERS_STREAMS_WRITERS_HPP
