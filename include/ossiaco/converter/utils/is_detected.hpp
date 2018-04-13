// Port of is_detected poached from cppreference.com
// TODO replace with Boost.Type Traits version

#ifndef OSSIACO_CONVERTER_IS_DETECTED_HPP
#define OSSIACO_CONVERTER_IS_DETECTED_HPP

#include <type_traits>

namespace Ossiaco::converter::stdxp {

namespace detail {

template<typename Default, typename AlwaysVoid, template<class...> class Op, class... Args>
struct detector {
    using value_t = std::false_type;
    using type    = Default;
};

template<typename Default, template<class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type    = Op<Args...>;
};

} // namespace detail

struct nonesuch {
    nonesuch()  = delete;
    ~nonesuch() = delete;

    nonesuch(const nonesuch&) = delete;
    nonesuch(nonesuch&&)      = delete;

    void operator=(const nonesuch&) = delete;
    void operator=(nonesuch&&) = delete;
};

template<template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template<template<class...> class Op, class... Args>
constexpr bool is_detected_v = detail::detector<nonesuch, void, Op, Args...>::value_t::value;

template<template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template<typename Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template<typename Default, template<class...> class Op, class... Args>
using detected_or_t = typename detail::detector<Default, void, Op, Args...>::type;

} // namespace Ossiaco::converter::stdxp

#endif // OSSIACO_CONVERTER_IS_DETECTED_HPP
