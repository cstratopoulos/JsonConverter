// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_TYPE_TREE_HPP
#define OSSIACO_CONVERTER_CORE_TYPE_TREE_HPP

#include <ossiaco/converter/utils/customized.hpp>
#include <ossiaco/converter/core/char_types.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>
#include <boost/type_traits/detected.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include <type_traits>

namespace Ossiaco::converter {

/// Customization point for enums that provide enum-mapped JSON conversion.
///
/// A `TypeTreeNode` for `Enum` is a structure that exposes a public `Map` type alias.
/// \requires `Enum` is an enum.
/// \see [providesTypeTreeBasic], [isTypeTreeLeaf], [isTypeTreeNonLeaf] for concept checking.
template<typename Enum>
struct TypeTreeNode : Default {
    /// Suggested helper alias for defining map entries.
    template<Enum e, typename T>
    using MapEntry = boost::mp11::mp_list<std::integral_constant<Enum, e>, T>;

    /// The type alias that must be exposed for generation of enum-to-type maps.
    ///
    /// A valid `Map` is a nonempty list of `MapEntry<e, T>` where `T` is either
    /// - a class/struct; or
    /// -an enumeration SubEnum such that `TypeTreeNode<SubEnum>` is a valid `TypeTreeNode`.
    using Map = void;

    /// The name of the JSON member field from which type info should be extracted.
    static constexpr CharType* typeFieldName() = delete;

    /// The default value that should be used for a JSON entry which is not in `Map`.
    ///
    /// `Map` must contain `MapEntry<defaultVal(), T>` for some `T` to enable deserialization
    /// when passed an unexpected enum value. If not provided, [InvalidEnumValue] is thrown.
    static constexpr Enum defaultVal() = delete;
};

/// The codomain of `TypeTreeNode<Enum>::Map`: all possible mapped classes.
template<typename Enum>
using TreeMappedClasses = boost::mp11::mp_unique<
    boost::mp11::mp_transform<
    boost::mp11::mp_second, typename TypeTreeNode<Enum>::Map
    >
>;

namespace detail {

//=== Helper traits for concept checking below ===//

template<typename Enum>
using TreeNodeMap = typename TypeTreeNode<Enum>::Map;

template<typename Enum>
using TreeNodeTypeField = decltype(TypeTreeNode<Enum>::typeFieldName());

template<typename Enum>
using TreeNodeDefaultVal = decltype(TypeTreeNode<Enum>::defaultVal());

template<typename MapEntry>
using IsPair = std::is_same<
    boost::mp11::mp_size<MapEntry>, boost::mp11::mp_size_t<2>>;

template <typename MapType>
using IsValidTreeMap = boost::mp11::mp_and<
    boost::mp11::mp_not<boost::mp11::mp_empty<MapType>>, // is nonempty
    boost::mp11::mp_is_map<MapType>,                     // is a map
    boost::mp11::mp_all_of<MapType, IsPair>              // all entries are pairs
>;

// Checks if TypeTreeNode<Enum> provides the basis for a valid TypeTreeNode specialization.
template<typename Enum>
using ProvidesTypeTreeBasic = boost::mp11::mp_and<
    Customized<TypeTreeNode<Enum>>,
    std::is_enum<Enum>,
    detail::IsValidTreeMap<boost::detected_t<TreeNodeMap, Enum>>,
    boost::is_detected_convertible<string_view_t, TreeNodeTypeField, Enum>
>;

// Checks if a TypeTreeNode MapType is a leaf: none of its mapped values are enums.
// This check should not be used in isolation as it will return true on an empty list.
template <typename Enum>
using IsTreeMapLeaf = boost::mp11::mp_none_of<
    boost::mp11::mp_transform<
        boost::mp11::mp_second,
        boost::detected_t<TreeNodeMap, Enum>>,
    std::is_enum
>;

} // namespace detail

template<typename Enum>
constexpr bool providesTypeTreeBasic = detail::ProvidesTypeTreeBasic<Enum>::value;

template<typename Enum>
constexpr bool isTypeTreeLeaf = std::conjunction_v<
    detail::ProvidesTypeTreeBasic<Enum>,
    detail::IsTreeMapLeaf<Enum>
>;

template<typename Enum>
constexpr bool isTypeTreeNonLeaf = std::conjunction_v<
    detail::ProvidesTypeTreeBasic<Enum>,
    std::negation<detail::IsTreeMapLeaf<Enum>>
>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_TYPE_TREE_HPP
