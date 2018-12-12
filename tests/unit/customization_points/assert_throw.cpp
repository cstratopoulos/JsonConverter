// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// This test uses Boost.Assert to intercept failed RapidJSON assertions, throwing them
// as exceptions instead.

#define RAPIDJSON_ASSERT BOOST_ASSERT
#define BOOST_ENABLE_ASSERT_HANDLER

#include <boost/assert.hpp>

#include "../types/person.hpp"

#include <catch2/catch.hpp>

namespace boost {

void assertion_failed(const char* expr, const char* function, const char* file, long line)
{
    throw Ossiaco::converter::RapidJsonAssert(expr, function, file, line);
}

}

SCENARIO("Throwing exceptions from JSON type errors", "[RapidJsonAssert]")
{
    WHEN("A JSON field that should be string has int type")
    {

        const auto json = TSTR(R"--({
"name": 1234,
"city": "MTL"
})--");
        CAPTURE(json);
        THEN("Attempting to deserialize from it throws a RapidJsonAssert")
        {
            CHECK_THROWS_AS(
                Ossiaco::converter::JsonDeserializer<test_types::Person>::fromString(json),
                Ossiaco::converter::RapidJsonAssert);
        }

        THEN("The exception thrown from deserializing it can be caught as SerializationException")
        {
            CHECK_THROWS_AS(
                Ossiaco::converter::JsonDeserializer<test_types::Person>::fromString(json),
                Ossiaco::converter::SerializationException);
        }
    }
}
