// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SPRINT_RECORD_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SPRINT_RECORD_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/components/vocab_type.hpp>

#include <date/date.h>

#include <chrono>
#include <type_traits>

namespace test_types {

template<bool pretty>
class SprintRecord {
    using string_t = Ossiaco::converter::string_t;
public:
    using DobType = std::conditional_t<pretty, date::year_month_day, date::sys_days>;

    SprintRecord(Ossiaco::converter::string_view_t name, DobType dob, std::chrono::milliseconds recTime)
        : _runnerName(name), _dateOfBirth(dob), _100mTime(recTime)
    {}

    SprintRecord() = default;
    SprintRecord(const SprintRecord&) = default;
    SprintRecord(SprintRecord&&) = default;

    SprintRecord& operator=(const SprintRecord&) = default;
    SprintRecord& operator=(SprintRecord&&) = default;

    ~SprintRecord() = default;

    const auto& runnerName() const { return _runnerName; }
    const auto dateOfBirth() const { return _dateOfBirth; }
    const auto recordTime() const { return _100mTime;  }

    static constexpr auto chronoProps()
    {
        using namespace Ossiaco::converter;
        if constexpr (pretty) {
            return boost::hana::make_basic_tuple(
                requiredProperty(
                    chronoFmtPair(&SprintRecord::_dateOfBirth, TSTR("%Y-%m-%d")),
                    TSTR("DOB")),
                requiredProperty(
                    chronoFmtPair(&SprintRecord::_100mTime, TSTR("%S seconds")),
                    TSTR("100mTime")));
        } else {
            return boost::hana::make_basic_tuple(
                requiredProperty(&SprintRecord::_dateOfBirth, TSTR("DOB")),
                requiredProperty(&SprintRecord::_100mTime, TSTR("100mTime")));
        }
    }

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        SprintRecord,
        (&SprintRecord::_runnerName, TSTR("runnerName"))
        (&SprintRecord::chronoProps));

    friend bool operator==(const SprintRecord& lhs, const SprintRecord& rhs)
    {
        return std::tie(lhs._runnerName, lhs._dateOfBirth, lhs._100mTime) ==
            std::tie(rhs._runnerName, rhs._dateOfBirth, rhs._100mTime);
    }

private:
    string_t _runnerName;

    DobType _dateOfBirth;

    std::chrono::milliseconds _100mTime;
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_SPRINT_RECORD_HPP
