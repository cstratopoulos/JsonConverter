// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_PERSON_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_PERSON_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/core/char_types.hpp>

#include <tuple>

namespace test_types {

class Person {
public:
    Person(Ossiaco::converter::string_view_t name, Ossiaco::converter::string_view_t city)
        : _name(name), _city(city)
    {}

    Person(const Person&) = delete;
    Person(Person&&) = default;

    Person& operator=(const Person&) = delete;
    Person& operator=(Person&&) = default;

    ~Person() = default;

    const auto& name() const { return _name; }
    const auto& city() const { return _city; }

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Person,
        (&Person::_name, OSSIACO_XPLATSTR("name"))
        (&Person::_city, OSSIACO_XPLATSTR("city")));

    friend bool operator==(const Person& lhs, const Person& rhs)
    {
        return std::tie(lhs._name, lhs._city) == std::tie(rhs._name, rhs._city);
    }

private:
    // Will be accessible to converter machinery.
    Person() = default;

    Ossiaco::converter::string_t _name;
    Ossiaco::converter::string_t _city;
};

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_PERSON_HPP
