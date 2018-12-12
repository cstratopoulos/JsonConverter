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
#include "../types/sprint_record.hpp"

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>

#include <catch2/catch.hpp>

namespace tt = test_types;

TEST_CASE(
    "Converting sprint records with duration and DOB",
    "[date][chrono][ConvertVocabType][ChronoPropertyConverter][SimpleTypeAllocator]")
{
    using namespace boost::mp11;

    using namespace std::literals::chrono_literals;
    using namespace date::literals;

    namespace Oc = Ossiaco::converter;

    mp_for_each<mp_list<mp_false, mp_true>>([](auto Bool) {
        static constexpr bool pretty = decltype(Bool)::value;

        const auto testName = [=](std::string_view sprinter) {
            std::ostringstream result;
            result << "Converting " << sprinter << "'s 100m record "
                   << (pretty ? "(pretty)" : "(terse)");

            return result.str();
        };

        const auto usain    = testName("Usain Bolt");
        const auto florence = testName("Florence Griffith-Joyner");

        runTestCases(
            makeSimpleStringTest(
                usain,
                [] {
                    return tt::SprintRecord<pretty>(
                        TSTR("Usain Bolt"), 1986_y / date::aug / 21, 9580ms);
                }),
            makeSimpleStringTest(florence, [] {
                return tt::SprintRecord<pretty>(
                    TSTR("Florence Griffith-Joyner"), 1959_y / date::dec / 21, 10490ms);
            }));
    });
}
