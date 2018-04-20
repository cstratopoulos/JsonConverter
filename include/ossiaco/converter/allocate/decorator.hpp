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

template<typename>
class JsonConverter;

template<typename Converter>
class PolyDecoratorAllocator : Inconstructible {
public:
    static_assert(isSpecialization<Converter, JsonConverter>);

    using ConverterProperties = traits::ConverterProperties<Converter>;
    using SubjectType         = typename ConverterProperties::SubjectType;

    template<typename Derived, typename Encoding>
    static bool registerDerivedClass();

    template<typename Writer>
    static void putDecorator(const SubjectType&, Writer&);

    template<typename Encoding>
    static auto resolveTypeAllocator(const rapidjson::GenericValue<Encoding>&);

private:
    template<typename Encoding>
    static auto& safeGetMapping()
    {
        static std::map<string_t, TypeAllocator<SubjectType, Encoding>> mappings;
        return mappings;
    }
};

template<typename Converter>
template<typename Derived, typename Encoding>
bool PolyDecoratorAllocator<Converter>::registerDerivedClass()
{
    adlInvokeDecoratorHook<SubjectType, Derived>();

    safeGetMapping<Encoding>().emplace(
        printTypeName<Derived>(), TypeAllocator<SubjectType, Encoding>::template make<Derived>());

    return Converter::template ensureRegisteredWithBase<Derived, Encoding>();
}

template<typename Converter>
template<typename Writer>
void PolyDecoratorAllocator<Converter>::putDecorator(const SubjectType&, Writer& writer)
{
    writer.String(OSSIACO_XPLATSTR("@type"));
    writer.String(printTypeName<SubjectType>());
}

template<typename Converter>
template<typename Encoding>
auto PolyDecoratorAllocator<Converter>::resolveTypeAllocator(
    const rapidjson::GenericValue<Encoding>& jsonValue)
{
    const CharType* name = nullptr;
    if (auto typeMember = jsonValue.FindMember(OSSIACO_XPLATSTR("@type")); typeMember != jsonValue.MemberEnd())
        name = typeMember->value.GetString();

    if (name == nullptr || printTypeName<SubjectType>() == name) {
        if constexpr(std::is_abstract_v<SubjectType>)
            throw AbstractTypeAllocation<SubjectType>();
        else
            return TypeAllocator<SubjectType, Encoding>::template make<>();
    }

    const auto& mappings = safeGetMapping<Encoding>();
    if (auto itr = mappings.find(name); itr != mappings.end())
        return itr->second;

    throw UnregisteredType<SubjectType>(name);
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_ALLOCATE_DECORATOR_HPP
