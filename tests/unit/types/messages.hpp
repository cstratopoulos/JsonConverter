// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/mapped.hpp>

#include <iostream>
#include <iomanip>

namespace test_types {

class Message {
public:
    enum class Type {
        unknown,
        chat,
        log
    };

    using string_t      = Ossiaco::converter::string_t;
    using string_view_t = Ossiaco::converter::string_view_t;
    using TimePointType = date::sys_time<std::chrono::milliseconds>;

    static TimePointType nowFloor()
    {
        return date::floor<typename TimePointType::duration>(std::chrono::system_clock::now());
    }

    explicit Message(
        string_view_t text = {}, TimePointType stamp = nowFloor(), Type type = Type::unknown)
        : _text(text), _timeStamp(stamp), _type(type)
    {}

    Message(const Message&) = default;
    Message(Message&&) = default;

    Message& operator=(const Message&) = default;
    Message& operator=(Message&&) = default;

    virtual ~Message() = default;

    const string_t& text() const { return _text; }
    TimePointType timeStamp() const { return _timeStamp; }
    Type type() const { return _type; }

    bool operator==(const Message& other) const
    {
        return std::tie(_text, _timeStamp, _type) ==
               std::tie(other._text, other._timeStamp, other._type);
    }

    OSSIACO_CONVERTER_BASE_MAPPED_SUPPORTED(
        Message, Type,
        (&Message::_text,      OSSIACO_XPLATSTR("text"))
        (&Message::_timeStamp, OSSIACO_XPLATSTR("timeStamp"))
        (&Message::_type,      OSSIACO_XPLATSTR("type")));

private:
    string_t _text{};
    TimePointType _timeStamp{nowFloor()};
    Type _type{Type::unknown};
};

class ChatMessage : public Message {
public:
    explicit ChatMessage(
        string_view_t sender = {},
        string_view_t text   = {},
        TimePointType stamp  = Message::nowFloor())
        : Message(text, stamp, Type::chat), _sender(sender)
    {}

    ChatMessage(const ChatMessage&) = default;
    ChatMessage(ChatMessage&&) = default;

    ChatMessage& operator=(const ChatMessage&) = default;
    ChatMessage& operator=(ChatMessage&&) = default;

    ~ChatMessage() = default;

    const string_t& sender() const { return _sender; }

    bool operator==(const ChatMessage& other) const
    {
        return (static_cast<const Message&>(*this) == static_cast<const Message&>(other)) &&
               (_sender == other._sender);
    }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        ChatMessage, Message, 
        (&ChatMessage::_sender, OSSIACO_XPLATSTR("sender")));

private:
    string_t _sender{};
};

class LogMessage : public Message {
public:
    enum class Level {
        debug,
        info,
        warn,
        error,
        fatal
    };

    LogMessage(
        Level level         = Level::info,
        string_view_t text  = {},
        TimePointType stamp = Message::nowFloor())
        : Message(text, stamp, Type::log), _level(level)
    {}

    LogMessage(const LogMessage&) = default;
    LogMessage(LogMessage&&) = default;

    LogMessage& operator=(const LogMessage&) = default;
    LogMessage& operator=(LogMessage&&) = default;

    ~LogMessage() = default;

    Level level() const { return _level; }

    bool operator==(const LogMessage& other) const
    {
        return (static_cast<const Message&>(*this) == static_cast<const Message&>(other)) &&
               (_level == other._level);
    }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        LogMessage, Message,
        (&LogMessage::_level, OSSIACO_XPLATSTR("level")));

private:
    Level _level{Level::info};
};

} // namespace test_types

namespace Ossiaco::converter {

template<>
struct TypeTreeNode<test_types::Message::Type> {
    using MType = test_types::Message::Type;

    template<MType m, typename T>
    using MapEntry = boost::mp11::mp_list<std::integral_constant<MType, m>, T>;

    using Map = boost::mp11::mp_list<
        MapEntry<MType::unknown, test_types::Message>,
        MapEntry<MType::chat,    test_types::ChatMessage>,
        MapEntry<MType::log,     test_types::LogMessage>
    >;

    static constexpr auto typeFieldName() { return OSSIACO_XPLATSTR("type"); }
    static constexpr auto defaultVal() { return MType::unknown; }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES_HPP
