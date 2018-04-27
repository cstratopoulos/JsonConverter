// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_DISPATCH_HPP
#define OSSIACO_CONVERTER_ALLOCATE_DISPATCH_HPP

#include <ossiaco/converter/core/traits.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/type_traits/is_detected.hpp>

#include <type_traits>

namespace Ossiaco::converter {

template<typename>
struct SimpleTypeAllocator;

template<typename>
class MappedTypeAllocator;

template<typename>
struct EnumTypeMap;

template<typename>
class PolyDecoratorAllocator;

namespace detail {

template<typename, typename Enable = void>
class JsonConverter;

namespace mp11 = boost::mp11;

template<typename T>
using EnumDetected = mp11::mp_bool<traits::jsonEnumDetected<T>>;

// Type list performing allocator backend dispatch
template<typename T>
using AllocMap = mp11::mp_list<
    mp11::mp_list<EnumDetected<T>,        mp11::mp_quote<MappedTypeAllocator>>,
    mp11::mp_list<std::is_polymorphic<T>, mp11::mp_quote<PolyDecoratorAllocator>>
>;

template<typename T>
struct AllocTypeChecked {
    static_assert(mp11::mp_or<EnumDetected<T>, std::is_polymorphic<T>>::value,
        "Class does not provide expected traits for polymorphic allocator backend deduction");

    static_assert(traits::isExpectedJsonSupportTag<T, traits::PolySupportTag>);

    using QuotedType = mp11::mp_second<
        mp11::mp_at<
            AllocMap<T>,
            mp11::mp_find_if<AllocMap<T>, mp11::mp_front>>
    >;

    using Type = mp11::mp_invoke<QuotedType, detail::JsonConverter<T, void>>;
};

} // namespace detail

template<typename Class>
using PolyAllocBackend = typename detail::AllocTypeChecked<Class>::Type;

// Helper variable template for debugging allocator backend dispatch.
template<typename Class, template<typename> typename Backend>
constexpr bool expectedAllocBackend = std::is_same_v<
    PolyAllocBackend<Class>,
    Backend<detail::JsonConverter<Class, void>>
>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_DISPATCH_HPP
