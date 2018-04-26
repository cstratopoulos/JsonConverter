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

}

namespace detail {

namespace mp11 = boost::mp11;

template<typename T>
using EnumDetected = boost::is_detected<traits::JsonEnumType>;

// Type list performing allocator backend dispatch
template<typename T>
using AllocMap = mp11::mp_list<
    mp11::mp_list<EnumDetected<T>,            mp11::mp_quote<MappedTypeAllocator>>,
    mp11::mp_list<std::is_polymorphic<T>,     mp11::mp_quote<PolyDecoratorAllocator>>,
    mp11::mp_list<mp11::mp_true,              mp11::mp_quote<SimpleTypeAllocator>>
>;

// The allocator backend for `T`; called `Raw` because it is a quoted metafunction
template<typename T>
using AllocTypeRaw = mp11::mp_second<
    mp11::mp_at<
        AllocMap<T>,
        mp11::mp_find_if<AllocMap<T>, mp11::mp_front>>
>;

// The actual allocation backend for `T`.
// For `SimpleTypeAllocator` this is just `SimpleTypeAllocator`.
// For the other cases it is `AllocTypeRaw<T>::fn<JsonConverter<T>>`, i.e.,
// the allocator type templated on `JsonConverter<T>` since the polymorphic allocation backends
// are CRTP bases.
template<typename T>
using AllocType = mp11::mp_eval_if_c<
    std::is_same_v<AllocTypeRaw<T>, mp11::mp_quote<SimpleTypeAllocator>>,
    SimpleTypeAllocator<T>,
    mp11::mp_invoke, AllocTypeRaw<T>, detail::JsonConverter<T, void>
>;

} // namespace detail

template<typename Class>
using DeducedAllocBackend = detail::AllocType<Class>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_DISPATCH_HPP
