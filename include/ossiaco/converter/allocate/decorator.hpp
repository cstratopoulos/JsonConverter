// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ALLOCATE_DECORATOR_HPP
#define OSSIACO_CONVERTER_ALLOCATE_DECORATOR_HPP

#include <ossiaco/converter/allocate/type_allocator.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/exceptions.hpp>
#include <ossiaco/converter/core/traits.hpp>
#include <ossiaco/converter/hooks/logging.hpp>
#include <ossiaco/converter/utils/print_type_name.hpp>
#include <ossiaco/converter/utils/inconstructible.hpp>

#include <map>

namespace Ossiaco::converter {

template<typename Converter>
class PolyDecoratorAllocator : Inconstructible {
public:
    template<typename Class, typename Derived, typename Encoding>
    static bool registerDerivedClass();

    template<typename Class, typename Writer>
    static void putDecorator(const Class&, Writer&);

    template<typename Class, typename Encoding>
    static auto resolveTypeAllocator(const rapidjson::GenericValue<Encoding>&);

private:
    template<typename Class, typename Encoding>
    static auto& safeGetMapping()
    {
        static std::map<string_t, TypeAllocator<Class, Encoding>> mappings;
        return mappings;
    }
};

template<typename Converter>
template<typename Class, typename Derived, typename Encoding>
bool PolyDecoratorAllocator<Converter>::registerDerivedClass()
{
    adlInvokeDecoratorHook<Class, Derived>();

    safeGetMapping<Class, Encoding>().emplace(
        printTypeName<Derived>(), TypeAllocator<Class, Encoding>::template make<Derived>());

    return Converter::template ensureRegisteredWithBase<Derived, Encoding>();
}

template<typename Converter>
template<typename Class, typename Writer>
void PolyDecoratorAllocator<Converter>::putDecorator(const Class&, Writer& writer)
{
    writer.String(OSSIACO_XPLATSTR("@type"));
    writer.String(printTypeName<Class>());
}

template<typename Converter>
template<typename Class, typename Encoding>
auto PolyDecoratorAllocator<Converter>::resolveTypeAllocator(
    const rapidjson::GenericValue<Encoding>& jsonValue)
{
    const CharType* name = nullptr;
    if (auto typeMember = jsonValue.FindMember(OSSIACO_XPLATSTR("@type")); typeMember != jsonValue.MemberEnd())
        name = typeMember->value.GetString();

    if (name == nullptr || printTypeName<Class>() == name) {
        if constexpr(std::is_abstract_v<Class>)
            throw AbstractTypeAllocation<Class>();
        else
            return TypeAllocator<Class, Encoding>::template make<>();
    }

    const auto& mappings = safeGetMapping<Class, Encoding>();
    if (auto itr = mappings.find(name); itr != mappings.end())
        return itr->second;

    throw UnregisteredType<Class>(name);
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_DECORATOR_HPP
