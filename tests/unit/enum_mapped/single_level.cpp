// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Polymorphic conversion of enum-mapped types in a simple hierarchy with only a single level of type mapping

#include "../test_utils.hpp"
#include "../types/messages.hpp"
#include "../types/chess.hpp"

#include <catch2/catch.hpp>

namespace tt = test_types;
namespace Oc = Ossiaco::converter;

namespace mp11 = boost::mp11;

template<typename T>
using ExpectedBackend = mp11::mp_bool<Oc::expectedAllocBackend<T, Oc::MappedTypeAllocator>>;

template<typename Func>
auto makeMsgTest(std::string_view desc, Func&& func)
{
    return makeBasicTestCase<tt::Message>(
        desc,
        std::forward<Func>(func),
        makeObjectComparison<tt::Message>(),
        StringObjectConversion{});
}

template<typename Func>
auto makeChessTest(std::string_view desc, Func&& func)
{
    return makeBasicTestCase<tt::IChessPiece>(
        desc,
        std::forward<Func>(func),
        makeObjectComparison<tt::IChessPiece>(),
        StringObjectConversion{});
}

TEST_CASE("Message allocator dispatch/trait checks", "[MappedTypeAllocator]")
{
    STATIC_REQUIRE(
        mp11::mp_all_of<
            mp11::mp_transform<mp11::mp_second, typename Oc::TypeTreeNode<tt::Message::Type>::Map>,
            ExpectedBackend>::value);

    STATIC_REQUIRE(Oc::isTypeTreeLeaf<tt::Message::Type>);
}

TEST_CASE("Chess piece allocator dispatch checks", "[MappedTypeAllocator]")
{
    STATIC_REQUIRE(Oc::expectedAllocBackend<tt::IChessPiece, Oc::MappedTypeAllocator>);

    STATIC_REQUIRE(mp11::mp_all_of<
                      mp11::mp_transform<
                          mp11::mp_second,
                          typename Oc::TypeTreeNode<tt::IChessPiece::Type>::Map>,
                      ExpectedBackend>::value);

    STATIC_REQUIRE(Oc::isTypeTreeLeaf<tt::IChessPiece::Type>);
}

TEST_CASE("Converting a class with default value", "[MappedTypeAllocator]")
{

    runTestCases(
        makeMsgTest(
            "A chat message",
            [] { return tt::ChatMessage(TSTR("user"), TSTR("hello!")); }),
        makeMsgTest(
            "A log message",
            [] {
                return tt::LogMessage(
                    tt::LogMessage::Level::fatal, TSTR("a fatal error!!!!!"));
            }),
        makeMsgTest("A default/base message", [] {
            return tt::Message(TSTR("A simple message"));
        }));

    SECTION("Log message with invalid JSON enum value") {
        const auto jsonStr = TSTR(R"--({
"text": "Error happened",
"timeStamp": 1234567,
"type": -1,
"level": 3
})--");
        CAPTURE(jsonStr);

        SECTION("Polymorphic deserialization") {
            std::shared_ptr<tt::Message> message;

            REQUIRE_NOTHROW([&] {
                message = Oc::JsonDeserializer<tt::Message>::fromString(jsonStr);
            }());

            REQUIRE(message);
            CHECK_FALSE(dynamic_cast<const tt::ChatMessage*>(message.get()));
        }

        SECTION("Self-type deserialization") {
            std::shared_ptr<tt::LogMessage> message;

            REQUIRE_NOTHROW([&] {
                message = Oc::JsonDeserializer<tt::LogMessage>::fromString(jsonStr);
            }());

            CHECK(message);
        }
    }
}

TEST_CASE("Converting a class with no default and an abstract base", "[MappedTypeAllocator]")
{
    using PT = tt::IChessPiece::Type;

    SECTION("JSON string appearance") {
        tt::ChessPiece<PT::bishop> bishop(tt::IChessPiece::Color::black);

        const auto jsonStr = Oc::toJsonStringPretty(bishop);

        jsonCompare(
            jsonStr,
            TSTR(R"--({
"color": 0,
"type": 3,
"active": true
})--")
);
    }

    runTestCases(
        makeChessTest("A pawn", [] { return tt::ChessPiece<PT::pawn>{}; }),
        makeChessTest("A queen", [] { return tt::ChessPiece<PT::queen>{}; }),
        makeChessTest("A captured rook", [] { return tt::ChessPiece<PT::rook>(tt::IChessPiece::Color::white, false); })
    );

    SECTION("JSON with an invalid value") {
        const auto jsonStr = TSTR(R"--({
"color": 1,
"type": 1234,
"active": true
})--");
        CAPTURE(jsonStr);

        CHECK_THROWS_AS(Oc::JsonDeserializer<tt::IChessPiece>::fromString(jsonStr),
            Oc::InvalidEnumValue<tt::IChessPiece::Type>);

        CHECK_THROWS_AS(Oc::JsonDeserializer<tt::ChessPiece<PT::rook>>::fromString(jsonStr),
            Oc::InvalidEnumValue<tt::IChessPiece::Type>);
    }

    SECTION("JSON with no type field") {
        const auto jsonStr = TSTR(R"--({
"color": 0,
"active": false
})--");

        CAPTURE(jsonStr);

        using TypeFieldMissing = Oc::TypeFieldMissing<tt::IChessPiece, PT>;

        CHECK_THROWS_AS(
            Oc::JsonDeserializer<tt::IChessPiece>::fromString(jsonStr),
            TypeFieldMissing);
    }
}
