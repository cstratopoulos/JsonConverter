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
#include "../types/digraph.hpp"

namespace tt = test_types;

TEST_CASE("Directed graph with containers of smart pointers", "[shared_ptr][weak_ptr][ConvertSmartPtr]")
{
    runTestCases(makeSimpleStringTest("A path on three vertices", [] {
        return tt::Digraph({
            {OSSIACO_XPLATSTR("a"), OSSIACO_XPLATSTR("b")},
            {OSSIACO_XPLATSTR("b"), OSSIACO_XPLATSTR("c")},
        });
    }));
}