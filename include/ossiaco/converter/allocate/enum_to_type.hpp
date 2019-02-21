// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_ENUM_TO_TYPE_HPP
#define OSSIACO_CONVERTER_ALLOCATE_ENUM_TO_TYPE_HPP

#include <ossiaco/converter/allocate/type_allocator.hpp>
#include <ossiaco/converter/core/type_tree.hpp>
#include <ossiaco/converter/utils/customized.hpp>
#include <ossiaco/converter/utils/inconstructible.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <boost/mp11/map.hpp>
#include <boost/type_traits/detected.hpp>
#include <boost/type_traits/is_detected_exact.hpp>

#include <map>
#include <type_traits>

namespace Ossiaco::converter {

template<typename>
struct EnumTypeMap;

namespace detail {

// ConvertibleTo<Class>::fn<T> is true iff T is convertible to Class.
template<typename Class>
using ConvertibleTo = boost::mp11::mp_bind_back<std::is_convertible, Class>;

// True iff T* is convertible to Class* for all T in the range of TypeTreeNode<Enum>::Map
template<typename Enum, typename Class>
constexpr bool typeAllocCompatible = boost::mp11::mp_all_of_q<
    boost::mp11::mp_transform<
        std::add_pointer_t, TreeMappedClasses<Enum>>,
    ConvertibleTo<Class*>>::value;

template<typename Derived>
struct TypeAllocMapBase;

template<template<class, class, class> typename Derived, typename Class, typename Encoding, typename Enum>
class TypeAllocMapBase<Derived<Class, Encoding, Enum>> {
public:
    static_assert(providesTypeTreeBasic<Enum>);

    using AllocType = TypeAllocator<Class, Encoding>;

    using MapValType = std::conditional_t<
        isTypeTreeLeaf<Enum>,
        AllocType,
        std::add_pointer_t<AllocType(const rapidjson::GenericValue<Encoding>&)>
    >;

    using MapType = std::map<Enum, MapValType>;

    MapValType find(Enum e) const
    {
        auto findItr = _map.find(e);
        if (findItr == _map.end()) {
            if constexpr(boost::is_detected_exact_v<Enum, TreeNodeDefaultVal, Enum>)
                return _map.find(TypeTreeNode<Enum>::defaultVal())->second;
            else
                throw InvalidEnumValue<Enum>(e);
        }

        return findItr->second;
    }

private:
    const MapType _map{ [] {
        using namespace boost::mp11;

        MapType result;

        mp_for_each<typename TypeTreeNode<Enum>::Map>([&result](auto kvPair) {
            using KVPairType = decltype(kvPair);
            using ValType = mp_second<KVPairType>;

            result.emplace(
                mp_first<KVPairType>::value,
                Derived<Class, Encoding, Enum>::template makeMapVal<ValType>());
        });

        return result;
    }() };
};

template<typename Class, typename Encoding, typename Enum>
struct LeafTypeAllocMap : TypeAllocMapBase<LeafTypeAllocMap<Class, Encoding, Enum>> {
    static_assert(isTypeTreeLeaf<Enum>);


    template<typename ValType>
    static TypeAllocator<Class, Encoding> makeMapVal()
    {
        return TypeAllocator<Class, Encoding>::template make<
            std::conditional_t<std::is_convertible_v<ValType*, Class*>,
            ValType, Class>>();
    }

    TypeAllocator<Class, Encoding>
    resolveAllocatorImpl(Enum e, const rapidjson::GenericValue<Encoding>&) const
    {
        return this->find(e);
    }
};

template<typename Class, typename Encoding, typename Enum>
struct NonLeafTypeAllocMap : TypeAllocMapBase<NonLeafTypeAllocMap<Class, Encoding, Enum>> {
    static_assert(isTypeTreeNonLeaf<Enum>);

    using AllocType     = TypeAllocator<Class, Encoding>;
    using AllocCallable = std::add_pointer_t<AllocType(const rapidjson::GenericValue<Encoding>&)>;

    template<bool b>
    using AllocCallableRet = std::enable_if_t<b, AllocCallable>;

    template<typename ValType>
    static AllocCallableRet<std::is_enum_v<ValType>> makeMapVal()
    {
        return [](const rapidjson::GenericValue<Encoding>& jsonVal) {
            if constexpr(typeAllocCompatible<ValType, Class>)
                return EnumTypeMap<ValType>::template resolveAllocator<Class, Encoding>(jsonVal);
            else
                return TypeAllocator<Class, Encoding>::make();
        };
    }

    template<typename ValType>
    static AllocCallableRet<!std::is_enum_v<ValType>> makeMapVal()
    {
        return [](const rapidjson::GenericValue<Encoding>&) {
            return AllocType::template make<
                std::conditional_t<std::is_convertible_v<ValType*, Class*>,
                ValType, Class>>();
        };
    }

    TypeAllocator<Class, Encoding>
    resolveAllocatorImpl(Enum e, const rapidjson::GenericValue<Encoding>& jsonVal) const
    {
        return (this->find(e))(jsonVal);
    }
};

template<typename Class, typename Encoding, typename Enum>
using AllocMapImplType = std::conditional_t<
    isTypeTreeLeaf<Enum>,
    LeafTypeAllocMap<Class, Encoding, Enum>,
    NonLeafTypeAllocMap<Class, Encoding, Enum>
>;

} // namespace detail

// A mapping of Enum to allocators for class types.
// See static_assert statements for requirements on Enum.
template<typename Enum>
struct EnumTypeMap : Inconstructible {
    using TreeNode = TypeTreeNode<Enum>;

    static_assert(isTypeTreeLeaf<Enum> || isTypeTreeNonLeaf<Enum>);

    template<typename Class, typename Encoding>
    static TypeAllocator<Class, Encoding>
    resolveAllocator(const rapidjson::GenericValue<Encoding>& jsonValue)
    {
        string_view_t typeField = TreeNode::typeFieldName();

        auto typeFieldItr = jsonValue.FindMember(typeField.data());
        if (typeFieldItr == jsonValue.MemberEnd()) {
            throw TypeFieldMissing<Class, Enum>(typeField);
        }

        static const detail::AllocMapImplType<Class, Encoding, Enum> mapImpl{};

        return mapImpl.resolveAllocatorImpl(getValue<Enum>(typeFieldItr->value), jsonValue);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_ENUM_TO_TYPE_HPP
