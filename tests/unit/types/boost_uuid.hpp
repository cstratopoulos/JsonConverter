// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_BOOST_UUID_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_BOOST_UUID_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/components/custom_value.hpp>
#include <ossiaco/converter/components/dispatch.hpp>
#include <ossiaco/converter/utils/customized.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

namespace Ossiaco::converter {

template<typename String>
String UuidToString(const boost::uuids::uuid& uuid)
{
    if constexpr(std::is_same_v<String, std::string>)
        return boost::uuids::to_string(uuid);
    else
        return boost::uuids::to_wstring(uuid);
}

template<>
struct ConvertCustomValue<boost::uuids::uuid> {
    template<typename Encoding>
    static void fromJson(boost::uuids::uuid& uuid, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper&)
    {
        boost::uuids::string_generator gen;
        auto str = jsonValue.GetString();

        uuid = gen(str);
    }

    template<typename Writer>
    static void toJson(const boost::uuids::uuid& uuid, Writer& writer, ReferenceMapper&)
    {
        writer.String(UuidToString<string_t>(uuid));
    }
};

} // namespace Ossiaco::converter

namespace test_types {

struct UuidKey {
    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        UuidKey,
        (&UuidKey::_key, TSTR("key")));

    boost::uuids::uuid _key{};
};

bool operator==(const UuidKey& lhs, const UuidKey& rhs)
{
    return lhs._key == rhs._key;
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_BOOST_UUID_HPP
