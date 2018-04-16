// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_SIMPLE_HPP
#define OSSIACO_CONVERTER_ALLOCATE_SIMPLE_HPP

#include <ossiaco/converter/allocate/type_allocator.hpp>

#include <boost/type_traits/nonesuch.hpp>

namespace Ossiaco::converter {

struct SimpleTypeAllocator : boost::nonesuch {
    template<typename Class, typename Encoding>
    static constexpr auto resolveTypeAllocator(const rapidjson::GenericValue<Encoding>&)
    {
        return TypeAllocator<Class, Encoding>::template make<>();
    }
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_SIMPLE_HPP
