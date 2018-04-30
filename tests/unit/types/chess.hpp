// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_CHESS_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_CHESS_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/mapped.hpp>

#include <boost/mp11/list.hpp>

namespace test_types {

struct IChessPiece {
    enum class Color { black, white };
    enum class Type {
        pawn,
        knight,
        rook,
        bishop,
        queen,
        king,
    };

    IChessPiece(Color c, Type t, bool active)
        : _color(c), _type(t), _active(active)
    {}

    IChessPiece() = default;
    IChessPiece(const IChessPiece&) = default;
    IChessPiece(IChessPiece&&) = default;

    IChessPiece& operator=(const IChessPiece&) = default;
    IChessPiece& operator=(IChessPiece&&) = default;

    virtual ~IChessPiece() = default;

    virtual void move() = 0;

    bool operator==(const IChessPiece& other) const
    {
        return std::tie(_color, _type, _active) == std::tie(other._color, other._type, other._active);
    }

    OSSIACO_CONVERTER_BASE_MAPPED_SUPPORTED(
        IChessPiece, IChessPiece::Type,
        (&IChessPiece::_color,  OSSIACO_XPLATSTR("color"))
        (&IChessPiece::_type,   OSSIACO_XPLATSTR("type"))
        (&IChessPiece::_active, OSSIACO_XPLATSTR("active")));

    Color _color{Color::black};
    Type _type{Type::pawn};
    bool _active{true};
};

template<IChessPiece::Type pieceType>
class ChessPiece : public IChessPiece {
public:
    explicit ChessPiece(IChessPiece::Color color = IChessPiece::Color::black, bool active = true)
        : IChessPiece(color, pieceType, active)
    {}

    ChessPiece(const ChessPiece&) = default;
    ChessPiece(ChessPiece&&) = default;

    ChessPiece& operator=(const ChessPiece&) = default;
    ChessPiece& operator=(ChessPiece&&) = default;

    ~ChessPiece() = default;

    void move() override { /* piece-specific logic */}

    OSSIACO_CONVERTER_POLY_SUPPORTED(
        ChessPiece, IChessPiece, ());
};

} // namespace test_types

namespace Ossiaco::converter {

template<>
struct TypeTreeNode<test_types::IChessPiece::Type> {
    using PieceType = test_types::IChessPiece::Type;

    template<PieceType p>
    using MapEntry =
        boost::mp11::mp_list<std::integral_constant<PieceType, p>, test_types::ChessPiece<p>>;

    using Map = boost::mp11::mp_list<
        MapEntry<PieceType::pawn>,
        MapEntry<PieceType::knight>,
        MapEntry<PieceType::rook>,
        MapEntry<PieceType::bishop>,
        MapEntry<PieceType::queen>,
        MapEntry<PieceType::king>
    >;

    static constexpr const CharType* typeFieldName() { return OSSIACO_XPLATSTR("type"); }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_CHESS_HPP
