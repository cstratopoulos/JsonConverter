// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_TYPE_ALLOCATOR_HPP
#define OSSIACO_CONVERTER_ALLOCATE_TYPE_ALLOCATOR_HPP

#include <ossiaco/converter/serialize/deserializer.hpp>

#include <type_traits>

namespace Ossiaco::converter {

template<typename Class, typename Encoding>
class TypeAllocator {
    using allocPtrType       = std::add_pointer_t<std::unique_ptr<Class>()>;
    using deserializePtrType = std::add_pointer_t<
        void(Class&, const rapidjson::GenericValue<Encoding>&, ReferenceMapper&)>;

public:
    template<typename Derived = Class>
    static constexpr TypeAllocator make()
    {
        static_assert(std::is_base_of_v<Class, Derived>);
        return TypeAllocator<Class, Encoding>(allocateImpl<Derived>, deserializeImpl<Derived>);
    }

    std::unique_ptr<Class> allocate() const { return _allocator(); }

    void deserialize(
        Class& object,
        const rapidjson::GenericValue<Encoding>& jsonValue,
        ReferenceMapper& refs) const
    {
        _deserializer(object, jsonValue, refs);
    }

private:
    constexpr TypeAllocator(allocPtrType alloc, deserializePtrType deserial)
        : _allocator(alloc), _deserializer(deserial)
    {}

    template<typename Derived>
    static std::unique_ptr<Class> allocateImpl()
    {
        return std::unique_ptr<Class>(new Derived());
    }

    template<typename Derived>
    static void deserializeImpl(
        Class& object, const rapidjson::GenericValue<Encoding>& jsonValue, ReferenceMapper& refs)
    {
        JsonDeserializer<Derived>::fromJson(static_cast<Derived&>(object), jsonValue, refs);
    }

    const allocPtrType _allocator;
    const deserializePtrType _deserializer;
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_TYPE_ALLOCATOR_HPP
