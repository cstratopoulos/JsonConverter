// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES2_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES2_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/mapped.hpp>

namespace test_types::v2 {

class Message {
public:
    enum class Type {
        unknown,
        chat,
        log,
        command
    };

    using string_t = Ossiaco::converter::string_t;
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
        (&Message::_text, TSTR("text"))
        (&Message::_timeStamp, TSTR("timeStamp"))
        (&Message::_type, TSTR("type")));

private:
    string_t _text{};
    TimePointType _timeStamp{ nowFloor() };
    Type _type{ Type::unknown };
};

class ChatMessage : public Message {
public:
    explicit ChatMessage(
        string_view_t sender = {},
        string_view_t text = {},
        TimePointType stamp = Message::nowFloor())
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
        (&ChatMessage::_sender, TSTR("sender")));

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

    explicit LogMessage(
        Level level = Level::info,
        string_view_t text = {},
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
        (&LogMessage::_level, TSTR("level")));

private:
    Level _level{ Level::info };
};

enum class OpType {
    get,
    post,
    put,
    delete_
};

template<OpType operation>
class CommandMessage : public Message {
public:
    explicit CommandMessage(
        string_view_t target = {},
        string_view_t path   = {},
        TimePointType stamp  = Message::nowFloor())
        : Message(path, stamp, Type::command), _target(target)
    {}

    CommandMessage(const CommandMessage&) = default;
    CommandMessage(CommandMessage&&) = default;

    CommandMessage& operator=(const CommandMessage&) = default;
    CommandMessage& operator=(CommandMessage&&) = default;

    ~CommandMessage() = default;

    OpType op() const { return _op; }
    const string_t& target() const { return _target; }

    void exec() const { /* execute some command */ }

    bool operator==(const CommandMessage& other) const
    {
        return (static_cast<const Message&>(*this) == static_cast<const Message&>(other)) &&
            (std::tie(_op, _target) == std::tie(other._op, other._target));
    }

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        CommandMessage, Message,
        (&CommandMessage::_op,     TSTR("op"))
        (&CommandMessage::_target, TSTR("target")));

private:
    OpType _op{operation};
    string_t _target{};
};

} // namespace test_types::v2

namespace Ossiaco::converter {

namespace ttv2 = test_types::v2;

template<>
struct TypeTreeNode<ttv2::Message::Type> {
    using MType = ttv2::Message::Type;

    template<MType m, typename T>
    using MapEntry = boost::mp11::mp_list<std::integral_constant<MType, m>, T>;

    using Map = boost::mp11::mp_list<
        MapEntry<MType::unknown, ttv2::Message>,
        MapEntry<MType::chat,    ttv2::ChatMessage>,
        MapEntry<MType::log,     ttv2::LogMessage>,
        MapEntry<MType::command, ttv2::OpType>
    >;

    static constexpr auto typeFieldName() { return TSTR("type"); }
    static constexpr auto defaultVal() { return MType::unknown; }
};

template<>
struct TypeTreeNode<ttv2::OpType> {
    using Op = ttv2::OpType;

    template<Op op>
    using MapEntry = boost::mp11::mp_list<std::integral_constant<Op, op>, ttv2::CommandMessage<op>>;

    using Map = boost::mp11::mp_list<
        MapEntry<Op::get>,
        MapEntry<Op::post>,
        MapEntry<Op::put>,
        MapEntry<Op::delete_>
    >;

    static constexpr auto typeFieldName() { return TSTR("op"); }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_MESSAGES2_HPP
