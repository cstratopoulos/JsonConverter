// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP
#define OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP

#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/components/custom_value.hpp>
#include <ossiaco/converter/components/vocab_type.hpp>
#include <ossiaco/converter/utils/customized.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/map.hpp>

#include <memory>
#include <type_traits>

namespace Ossiaco::converter {

template<typename>
struct ConvertFirstClass;

template<typename>
struct ConvertLinearRange;

template<typename>
struct ConvertKeyValueRange;

template<typename>
struct ConvertSmartPtr;

namespace detail {

namespace mp11 = boost::mp11;

template<typename T>
using FirstClassConvertible = mp11::mp_bool<
    traits::primitiveConvertible<T> || 
    traits::jsonSupportDetected<T>
>;

template<typename T>
using DeducePointer = mp11::mp_or<
    SpecializationType<T, std::shared_ptr>,
    SpecializationType<T, std::weak_ptr>
>;

// Deduce the converter type for `Rng`, assuming unchecked that `Rng` models `InputRange`.
template<typename Rng>
using RangeConverterType = mp11::mp_eval_if<
    mp11::mp_not<ranges::view::keys_fn::Concept<Rng>>,
    ConvertLinearRange<Rng>,
    ConvertKeyValueRange, Rng
>;

template<typename Trait, template<typename> typename Converter>
using ConverterMapEntry = mp11::mp_list<Trait, mp11::mp_quote<Converter>>;

template<typename T>
using ConverterArchetypeMap = mp11::mp_list<
    ConverterMapEntry<FirstClassConvertible<T>,          ConvertFirstClass>,
    ConverterMapEntry<Customized<ConvertCustomValue<T>>, ConvertCustomValue>,
    ConverterMapEntry<Customized<ConvertVocabType<T>>,   ConvertVocabType>,
    ConverterMapEntry<ranges::InputRange<T>,             RangeConverterType>,
    ConverterMapEntry<DeducePointer<T>,                  ConvertSmartPtr>
>;

// Checked retrieval of matching map entry
template<typename T>
struct ArchetypeMapIndex {
    // The zero-indexed position of T's converter type in ConverterArchetypeMap<T>
    using Type = mp11::mp_find_if<ConverterArchetypeMap<T>, mp11::mp_front>;

    static_assert(
        !std::is_same_v<Type, mp11::mp_size<ConverterArchetypeMap<T>>>,
        "Couldn't categorize type under converter archetypes. Possible type error or unimplemented case.");
};

// Quoted version of converter to use
template<typename T>
using QuotedDeducedConverter = mp11::mp_second<
    mp11::mp_at<
        ConverterArchetypeMap<T>, 
        typename ArchetypeMapIndex<T>::Type>
>;

} // namespace detail

// This could just be an alias template but this way it can be forward declared where we need it in
// range converters.
template<typename Class>
struct ConverterDeductor {
    using Type = boost::mp11::mp_invoke<
        detail::QuotedDeducedConverter<Class>, Class>;
};

// The component converter that shall be used to convert Class.
template<typename Class>
using DeducedConverterType = typename ConverterDeductor<Class>::Type;

// Helper variable template for debugging converter component dispatch.
template<typename Class, template<typename> typename ConverterComponent>
constexpr bool expectedConverterComponent = std::is_same_v<
    DeducedConverterType<Class>,
    ConverterComponent<Class>
>;

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_COMPONENTS_DISPATCH_HPP
