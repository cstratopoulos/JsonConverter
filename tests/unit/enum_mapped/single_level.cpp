// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#include "../test_utils.hpp"
#include "../types/chess.hpp"

#include <catch.hpp>

namespace tt = test_types;
namespace Oc = Ossiaco::converter;

namespace mp11 = boost::mp11;

template<typename ChessPieceType>
using ExpectedBackend = mp11::mp_bool<Oc::expectedAllocBackend<ChessPieceType, Oc::MappedTypeAllocator>>;

TEST_CASE("Allocator dispatch checks", "[MappedTypeAllocator]")
{
    static_assert(Oc::expectedAllocBackend<tt::IChessPiece, Oc::MappedTypeAllocator>);

    static_assert(
        mp11::mp_all_of<
            mp11::mp_transform<
                mp11::mp_second, typename Oc::TypeTreeNode<tt::IChessPiece::Type>::Map>,
            ExpectedBackend
        >::value);

    static_assert(Oc::isTypeTreeLeaf<tt::IChessPiece::Type>);

    SUCCEED("Mapped type allocator static asserts passed");
}