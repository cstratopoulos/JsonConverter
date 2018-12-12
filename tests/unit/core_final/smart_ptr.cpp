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

TEST_CASE(
    "Directed graph with containers of smart pointers", "[shared_ptr][weak_ptr][ConvertSmartPtr]")
{
    runTestCases(
        makeSimpleStringTest(
            "A path on three vertices",
            [] {
                return tt::Digraph({
                    {TSTR("a"), TSTR("b")},
                    {TSTR("b"), TSTR("c")},
                });
            }),
        makeSimpleStringTest(
            "A loop vertex",
            [] {
                return tt::Digraph({{TSTR("self"), TSTR("self")}});
            }),
        makeSimpleStringTest(
            "A bidirectional edge",
            [] {
                return tt::Digraph({
                    {TSTR("here"), TSTR("there")},
                    {TSTR("there"), TSTR("here")},
                });
            }),
        makeSimpleStringTest("The complete directed graph on 4 vxs", [] {
            std::vector<Ossiaco::converter::string_t> vxs{TSTR("1"),
                                                          TSTR("2"),
                                                          TSTR("3"),
                                                          TSTR("4")};

            tt::Digraph g;

            for (const auto& source : vxs)
                for (const auto& sink : vxs)
                    if (source != sink)
                        g.addEdge(std::make_pair(source, sink));

            return g;
        }));
}
