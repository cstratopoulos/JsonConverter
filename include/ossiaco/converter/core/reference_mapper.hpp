// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP
#define OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP

#include <ossiaco/converter/core/exceptions.hpp>

#include <atomic>
#include <memory>
#include <optional>
#include <unordered_map>

#include <cstdint>

namespace Ossiaco::converter {

class ReferenceMapper {
public:
    std::optional<int32_t> at(uintptr_t key)
    {
        if (auto iterator = _map.find(key); iterator != _map.end())
            return iterator->second;

        return {};
    }

    int32_t add(uintptr_t key)
    {
        auto rv = _counter++;

        _map.emplace(key, rv);

        return rv;
    }

    template<typename PropertyType>
    std::shared_ptr<PropertyType> add(int32_t id, std::unique_ptr<PropertyType> value)
    {
        auto emplaceResult = _references.emplace(id, std::move(value));

        if (emplaceResult.second == false) {
            throw InvalidReferenceId(id);
        }

        return std::static_pointer_cast<PropertyType>((emplaceResult.first)->second);
    }

    template<typename PropertyType>
    std::shared_ptr<PropertyType> get(int32_t id) const
    {
        try {
            return std::static_pointer_cast<PropertyType>(_references.at(id));
        } catch (const std::out_of_range&) {
            throw InvalidReferenceId(id);
        }
    }

private:
    std::unordered_map<uintptr_t, int32_t> _map;

    std::unordered_map<int32_t, std::shared_ptr<void>> _references;

    std::atomic<int32_t> _counter{1};
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP
