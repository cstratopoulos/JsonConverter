// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP
#define OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP

#include <ossiaco/converter/core/exceptions.hpp>
#include <ossiaco/converter/core/pointer_wrapper.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>

#include <cstdint>

namespace Ossiaco::converter {

class ReferenceMapper {
public:
    ReferenceMapper()                       = default;
    ReferenceMapper(ReferenceMapper&&)      = default;
    ReferenceMapper(const ReferenceMapper&) = delete;

    ReferenceMapper& operator=(const ReferenceMapper&) = delete;
    ReferenceMapper& operator=(ReferenceMapper&&) = default;

    ~ReferenceMapper()
    {
        for (auto& ref : _references)
            delete ref.second;
    }

    int32_t at(uintptr_t key)
    {
        auto iterator = _map.find(key);

        return iterator == _map.end() ? 0 : iterator->second;
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
        auto tmp    = std::make_unique<PointerWrapper<PropertyType>>(std::move(value));
        auto result = std::static_pointer_cast<PropertyType>(tmp->value());

        _references.emplace(id, tmp.release());

        return result;
    }

    template<typename PropertyType>
    std::shared_ptr<PropertyType> get(int32_t id) const
    {
        try {
            return std::static_pointer_cast<PropertyType>(_references.at(id)->value());
        } catch (const std::out_of_range&) {
            throw InvalidReferenceId(id);
        }
    }

private:
    std::unordered_map<uintptr_t, int32_t> _map;

    // TODO possibly replace with boost::ptr_unordered_map<int32_t, PointerWrapperBase>
    std::unordered_map<int32_t, PointerWrapperBase*> _references;

    std::atomic<int32_t> _counter{1};
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_REFERENCE_MAPPER_HPP
