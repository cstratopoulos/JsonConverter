// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_NESTED_RANGES_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_NESTED_RANGES_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/components/ranges.hpp>

#include <boost/container/flat_map.hpp>

#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace test_types {


struct MapOfMaps {
    using string_t = Ossiaco::converter::string_t;

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        MapOfMaps,
        (&MapOfMaps::_mapMap, OSSIACO_XPLATSTR("mapMap")));

    std::unordered_map<string_t, std::map<string_t, std::list<string_t>>> _mapMap;
};

inline bool operator==(const MapOfMaps& lhs, const MapOfMaps& rhs)
{
    return lhs._mapMap == rhs._mapMap;
}

inline bool operator!=(const MapOfMaps& lhs, const MapOfMaps& rhs)
{
    return !(lhs == rhs);
}

class NumberMap {
public:
    NumberMap()                 = default;
    NumberMap(const NumberMap&) = default;
    NumberMap(NumberMap&&)      = default;

    NumberMap& operator=(const NumberMap&) = default;
    NumberMap& operator=(NumberMap&&) = default;

    ~NumberMap() = default;

    NumberMap(const std::set<unsigned>& numSet)
    {
        _vecNums.resize(*std::max_element(numSet.begin(), numSet.end()) + 1);

        for (unsigned i : numSet) {
            if constexpr (Ossiaco::converter::wcharUnicodeEnabled)
                _stringNums.emplace(std::to_wstring(i), i);
            else
                _stringNums.emplace(std::to_string(i), i);

            _vecNums[i] = std::vector<unsigned>(i, i);
        }
    }

    unsigned fromString(const Ossiaco::converter::string_t& str) const
    {
        return _stringNums.at(str);
    }

    const std::vector<unsigned>& getVec(unsigned i) const { return _vecNums.at(i); }

    friend bool operator==(const NumberMap& lhs, const NumberMap& rhs)
    {
        return std::tie(lhs._stringNums, lhs._vecNums) == std::tie(rhs._stringNums, rhs._vecNums);
    }

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        NumberMap,
        (&NumberMap::_stringNums, OSSIACO_XPLATSTR("stringNums"))
        (&NumberMap::_vecNums,    OSSIACO_XPLATSTR("vecNums")));

private:
    // A mapping of strings to numbers.
    boost::container::flat_map<Ossiaco::converter::string_t, unsigned> _stringNums;

    // A vector whose ith entry is either empty or an i-length vector of i's
    std::vector<std::vector<unsigned>> _vecNums;
};

inline bool operator!=(const NumberMap& lhs, const NumberMap& rhs) { return !(lhs == rhs); }

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_NESTED_RANGES_HPP
