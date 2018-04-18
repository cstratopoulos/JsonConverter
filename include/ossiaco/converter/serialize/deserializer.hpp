// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_SERIALIZE_DESERIALIZER_HPP
#define OSSIACO_CONVERTER_SERIALIZE_DESERIALIZER_HPP

#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/exceptions.hpp>
#include <ossiaco/converter/core/reference_mapper.hpp>
#include <ossiaco/converter/utils/file_handle.hpp>

#include <boost/mp11/tuple.hpp>
#include <boost/type_traits/nonesuch.hpp>
#include <rapidjson/document.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/filereadstream.h>

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

namespace Ossiaco::converter {

template<typename>
class JsonConverter;

template<typename Class, typename Encoding = utf_t>
class JsonDeserializer : boost::nonesuch {
public:
    static std::shared_ptr<Class> fromString(string_view_t);
    static std::shared_ptr<Class> fromString(CharType*);

    static std::shared_ptr<Class> fromFile(const CharType* fileName);

    static void fromJson(Class&, const rapidjson::GenericValue<Encoding>&, ReferenceMapper&);

private:
    static std::shared_ptr<Class> deserializeStream(rapidjson::GenericDocument<Encoding>&);
};

template<typename Class, typename Encoding>
std::shared_ptr<Class> JsonDeserializer<Class, Encoding>::fromString(string_view_t jsonStr)
{
    std::vector<CharType> mutableBuf;

    mutableBuf.reserve(jsonStr.length() + 1);
    std::copy(jsonStr.begin(), jsonStr.end(), std::back_inserter(mutableBuf));
    mutableBuf.push_back(OSSIACO_XPLATSTR('\0'));

    return fromString(mutableBuf.data());
}

template<typename Class, typename Encoding>
std::shared_ptr<Class> JsonDeserializer<Class, Encoding>::fromString(CharType* jsonStr)
{
    rapidjson::GenericDocument<Encoding> jsonValue;
    jsonValue.template ParseInsitu<rapidjson::kParseValidateEncodingFlag>(jsonStr);

    return deserializeStream(jsonValue);
}

template<typename Class, typename Encoding>
std::shared_ptr<Class> JsonDeserializer<Class, Encoding>::fromFile(const CharType* fileName)
{
    UniqueFileHandle fp(uniqueHandleFOpen(fileName, OSSIACO_XPLATSTR("rb")));
    std::array<char, 65536> buffer;

    namespace rj = rapidjson;

    rj::FileReadStream bufferInStream(fp.get(), buffer.data(), buffer.size());
    rj::AutoUTFInputStream<unsigned, rj::FileReadStream> encodedInStream(bufferInStream);
    rj::GenericDocument<Encoding> doc;

    doc.template ParseStream<rj::kParseValidateEncodingFlag, rj::AutoUTF<unsigned>>(encodedInStream);

    return deserializeStream(doc);
}

template<typename Class, typename Encoding>
void JsonDeserializer<Class, Encoding>::fromJson(
    Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
{
    boost::mp11::tuple_for_each(JsonConverter<Class>::_properties, [&](auto prop) {
        prop.fromJson(object, jsonValue, refs);
    });
}

template<typename Class, typename Encoding>
std::shared_ptr<Class> JsonDeserializer<Class, Encoding>::deserializeStream(
    rapidjson::GenericDocument<Encoding>& jsonValue)
{
    if (jsonValue.HasParseError())
        throw RapidJsonParseError(rapidjson::ParseResult(jsonValue.GetParseError(), jsonValue.GetErrorOffset()));

    ReferenceMapper refs;
    auto resolver = JsonConverter<Class>::template resolveTypeAllocator<Class, Encoding>(jsonValue);

    std::unique_ptr<Class> tmp(resolver.allocate());
    std::shared_ptr<Class> result;

    if (auto member = jsonValue.FindMember(OSSIACO_XPLATSTR("@id")); member != jsonValue.MemberEnd())
        result = refs.add((member->value).GetInt(), std::move(tmp));
    else
        result = std::move(tmp);

    resolver.deserialize(*result, jsonValue, refs);

    return result;
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_SERIALIZE_DESERIALIZER_HPP
