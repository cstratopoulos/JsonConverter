// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
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

template<typename Map, typename Enum>
auto typeMapFind(const Map& m, Enum e)
{
    static_assert(std::is_enum_v<Enum>);

    auto findItr = m.find(e);
    if (findItr == m.end()) {
        if constexpr (boost::is_detected_exact_v<Enum, TreeNodeDefaultVal, Enum>)
            return m.find(TypeTreeNode<Enum>::defaultVal())->second;
        else
            throw InvalidEnumValue<Enum>(e);
    }

    return findItr->second;
}

template<typename Class, typename Encoding, typename Enum>
struct LeafTypeAllocMap {
    using AllocType = TypeAllocator<Class, Encoding>;
    using MapType   = std::map<Enum, AllocType>;

    AllocType find(Enum eType) const { return typeMapFind(_map, eType); }

    const MapType _map{[] {
        using namespace boost::mp11;

        MapType result;

        mp_for_each<typename TypeTreeNode<Enum>::Map>([&result](auto keyValList) {
            using KVPairType = decltype(keyValList);
            using ValType = mp_second<KVPairType>;

            result.emplace(
                mp_first<KVPairType>::value,
                AllocType::template make<
                    std::conditional_t<
                        std::is_convertible_v<ValType*, Class*>, 
                        ValType, Class>
                >());
        });

        return result;
    }()};
};

namespace free_callables {

template<typename Class, typename Encoding, typename ValType>
TypeAllocator<Class, Encoding> leafAllocCallable(const rapidjson::GenericValue<Encoding>& jsonVal)
{
    if constexpr(typeAllocCompatible<ValType, Class>)
        return EnumTypeMap<ValType>::template resolveAllocator<Class, Encoding>(jsonVal);
    else
        return TypeAllocator<Class, Encoding>::make();
}

template<typename Class, typename Encoding, typename ValType>
TypeAllocator<Class, Encoding> nonLeafAllocCallable(const rapidjson::GenericValue<Encoding>& jsonVal)
{
    using AllocType = TypeAllocator<Class, Encoding>;

    if constexpr(std::is_convertible_v<ValType*, Class*>)
        return AllocType::template make<ValType>();
    else
        return AllocType::make();
}

} // namespace free_callables

// The TypeAllocator map for a root or other interal node of a type tree.
// Maps Enum -> function pointer: TypeAllocator<Class, Encoding>(const GenericValue&)
template<typename Class, typename Encoding, typename Enum>
struct NonLeafTypeAllocMap {
    using AllocType     = TypeAllocator<Class, Encoding>;
    using AllocCallable = std::add_pointer_t<AllocType(const rapidjson::GenericValue<Encoding>&)>;
    using MapType       = std::map<Enum, AllocCallable>;

    template<typename ValType>
    static AllocCallable makeMapVal()
    {
        if constexpr(std::is_enum_v<ValType>)
            return &free_callables::leafAllocCallable<Class, Encoding, ValType>;
        else
            return &free_callables::nonLeafAllocCallable<Class, Encoding, ValType>;
    }

    AllocCallable find(Enum eType) const { return typeMapFind(_map, eType); }

    const MapType _map{[] {
        MapType result;

        using namespace boost::mp11;

        mp_for_each<typename TypeTreeNode<Enum>::Map>([&result](auto keyVal) {
            using KVPairType = decltype(keyVal);

            result[mp_first<KVPairType>::value] = makeMapVal<mp_second<KVPairType>>();
        });

        return result;
    }()};
};

template<typename Enum>
struct AllocMapWrapper {
    static_assert(providesTypeTreeBasic<Enum>);

    template<typename Class, typename Encoding>
    static TypeAllocator<Class, Encoding>
    resolveAllocatorImpl(Enum eVal, const rapidjson::GenericValue<Encoding>& jsonVal)
    {
        using AllocMapType = std::conditional_t<
            isTypeTreeLeaf<Enum>,
            LeafTypeAllocMap<Class, Encoding, Enum>,
            NonLeafTypeAllocMap<Class, Encoding, Enum>>;

        static const AllocMapType allocMap{};

        auto mapResult = allocMap.find(eVal);
        if constexpr (isTypeTreeLeaf<Enum>)
            return mapResult;
        else
            return mapResult(jsonVal);
    }
};

template<typename Enum, typename Class, typename Encoding>
using ResolveAllocImplType = decltype(
    AllocMapWrapper<Enum>::template resolveAllocatorImpl<Class, Encoding>(
        Enum(), std::declval<const rapidjson::GenericValue<Encoding>&>())
    );

} // namespace detail

// A mapping of Enum to allocators for class types.
// See static_assert statements for requirements on Enum.
template<typename Enum>
struct EnumTypeMap : Inconstructible {
    using MapImpl  = detail::AllocMapWrapper<Enum>;
    using TreeNode = TypeTreeNode<Enum>;

    static_assert(isTypeTreeLeaf<Enum> || isTypeTreeNonLeaf<Enum>);

    template<typename Class, typename Encoding>
    static TypeAllocator<Class, Encoding>
    resolveAllocator(const rapidjson::GenericValue<Encoding>& jsonValue)
    {
        static_assert(
            boost::is_detected_exact_v<
                TypeAllocator<Class, Encoding>,
                detail::ResolveAllocImplType, Enum, Class, Encoding>);

        string_view_t typeField = TreeNode::typeFieldName();

        auto typeFieldItr = jsonValue.FindMember(typeField.data());
        if (typeFieldItr == jsonValue.MemberEnd()) {
            throw TypeFieldMissing<Class, Enum>(typeField);
        }

        return MapImpl::template resolveAllocatorImpl<Class, Encoding>(
            getValue<Enum>(typeFieldItr->value), jsonValue);
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_ENUM_TO_TYPE_HPP
