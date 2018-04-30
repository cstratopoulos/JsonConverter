// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Polymorphic conversion in a type tree whose root node contains a link to a subtree

#include "../test_utils.hpp"
#include "../types/messages2.hpp"

#include <catch.hpp>

namespace ttv2 = test_types::v2;

namespace Oc   = Ossiaco::converter;

namespace mp11 = boost::mp11;

template<typename T>
using ExpectedBackend = mp11::mp_bool<Oc::expectedAllocBackend<T, Oc::MappedTypeAllocator>>;

template<typename Func>
auto makeMsgTest(std::string_view desc, Func&& func)
{
    return makeBasicTestCase<ttv2::Message>(
        desc,
        std::forward<Func>(func),
        makeObjectComparison<ttv2::Message>(),
        StringObjectConversion{});
}

TEST_CASE("Multi-level message allocator dispatch/trait checks", "[MappedTypeAllocator]")
{
    using LeafMsgTypes = mp11::mp_list<ttv2::Message, ttv2::LogMessage, ttv2::ChatMessage>;

    static_assert(mp11::mp_all_of<LeafMsgTypes, ExpectedBackend>::value);

    static_assert(
        mp11::mp_all_of<
            mp11::mp_transform<
                mp11::mp_second, typename Oc::TypeTreeNode<ttv2::OpType>::Map>,
            ExpectedBackend
        >::value);

    static_assert(Oc::isTypeTreeLeaf<ttv2::OpType>);
    static_assert(Oc::isTypeTreeNonLeaf<ttv2::Message::Type>);

    SUCCEED("Two-level message mapped conversion static asserts passed");
}

TEST_CASE("Converting a class in the second level of a type tree hierarchy", "[MappedTypeAllocator]")
{
    using Op = ttv2::OpType;

    SECTION("JSON string appearance") {
        const auto timeStamp = ttv2::Message::nowFloor();

        ttv2::CommandMessage<Op::post> postMsg(OSSIACO_XPLATSTR("www.example.com"), OSSIACO_XPLATSTR("/api/1234"), timeStamp);

        Oc::ostringstream_t stream;
        stream << OSSIACO_XPLATSTR(R"--({
"text": "/api/1234",
"timeStamp": )--")
               << std::chrono::time_point_cast<std::chrono::milliseconds>(timeStamp)
                      .time_since_epoch()
                      .count()
               << OSSIACO_XPLATSTR(R"--(,
"type": 3,
"op": 1,
"target": "www.example.com"
})--");

        const auto jsonStr = stream.str();
        jsonCompare(Oc::toJsonStringPretty(postMsg), jsonStr);
    }

    runTestCases(makeMsgTest("A GET request", [] {
        return ttv2::CommandMessage<Op::get>(
            OSSIACO_XPLATSTR("www.google.com"), OSSIACO_XPLATSTR("/"));
    }));

    SECTION("Command with invalid op field but valid type field")
    {
        const auto jsonStr = OSSIACO_XPLATSTR(R"--({
"text": "/",
"timeStamp": 12345,
"type": 3,
"op": -1,
"target": "www.example.com"
})--");

        CHECK_THROWS_AS(
            Oc::JsonDeserializer<ttv2::Message>::fromString(jsonStr),
            Oc::InvalidEnumValue<ttv2::OpType>);
    }
}