// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_ADAPT_HPP
#define OSSIACO_CONVERTER_ADAPT_HPP

#include <ossiaco/converter/allocate/type_allocator.hpp>
#include <ossiaco/converter/components/chrono_fmt_json_property.hpp>
#include <ossiaco/converter/components/json_property.hpp>
#include <ossiaco/converter/core/buffers_streams_writers.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/serialize.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>

#include <tuple>

namespace Ossiaco::converter {

// Sadly CTAD for alias templates does not exist so these helpers are still needed.

template<typename MemberPtr>
constexpr auto requiredProperty(MemberPtr member, const CharType* name)
{
    return RequiredJsonProperty<MemberPtr>(member, name);
}

template<typename MemberPtr>
constexpr auto optionalProperty(MemberPtr member, const CharType* name)
{
	return OptionalJsonProperty<MemberPtr>(member, name);
}

template<typename MemberPtr>
constexpr auto requiredProperty(ChronoFmtPair<MemberPtr> fmtPair, const CharType* name)
{
	return RequiredChronoJsonProperty<MemberPtr>(std::move(fmtPair), name);
}

template<typename MemberPtr>
constexpr auto optionalProperty(ChronoFmtPair<MemberPtr> fmtPair, const CharType* name)
{
    return OptionalChronoJsonProperty<MemberPtr>(std::move(fmtPair), name);
}

/// Helper class which exposes a chainable call operator for use in macros.
template<typename... Props>
struct PropertiesHelper {
    std::tuple<Props...> _props{};

    /// Template argument deduction helper.
    template<typename... NewProps>
    constexpr auto expand(std::tuple<NewProps...>&& newProps)
    {
        return PropertiesHelper<Props..., NewProps...>{
            std::tuple_cat(std::move(_props), std::move(newProps))};
    }

    /// No-op for providing empty properties.
    constexpr auto operator()() { return std::move(*this); }

	// Add a required JSON property by specifying a member pointer and property name.
	template<typename MemberPtr, typename = std::enable_if_t<std::is_member_pointer_v<MemberPtr>>>
	constexpr auto operator()(MemberPtr member, const CharType* name)
	{
		return expand(std::make_tuple(JsonProperty<MemberPtr>(member, name)));
	}

	// Add a JSON property with explicitly specified missing value policy.
	// e.g., with the helper functions above:
	//     `operator()(optionalProperty(member, name))`
	template<typename MemberPtr, NotFoundHandlerPtr notFound>
	constexpr auto operator()(JsonProperty<MemberPtr, notFound> prop)
	{
		return expand(std::make_tuple(std::move(prop)));
	}

    // Add a required formatted chrono JSON property by specifying a ChronoFmtPair and name.
    template<typename MemberPtr>
    constexpr auto operator()(ChronoFmtPair<MemberPtr> chronoPair, const CharType* name)
    {
        return expand(std::make_tuple(ChronoJsonProperty<MemberPtr>(std::move(chronoPair), name)));
    }

	// Add a chrono formatted JSON property with explicitly specified missing value policy.
    //
    // Sample usage with the helper functions above:
    //     `operator()(optionalProperty(chronoFmtPair(member, format), name))`
    template<typename MemberPtr, NotFoundHandlerPtr notFound>
    constexpr auto operator()(ChronoJsonProperty<MemberPtr, notFound> propConv)
    {
        return expand(std::make_tuple(std::move(propConv)));
    }

    /// Add a tuple of properties via function pointer.
    ///
    /// This overload can be convenient e.g., for inheriting properties from a CRTP base
    /// which is not itself used for polymorphic conversion.
    /// \requires `ReturnType` is a [std::tuple].
    template<typename ReturnType>
    constexpr auto operator()(ReturnType(*func)())
    {
        static_assert(isSpecialization<ReturnType, std::tuple>);

        return expand(func());
    }
};

} // namespace Ossiaco::converter

// The signature of a `toJson` method for `_writerType`.
#define OSSIACO_INTERNAL_TOJSON_METHOD_SIG(_writerType)                                            \
    void toJson(                                                                                   \
        _writerType& writer,                                                                       \
        Ossiaco::converter::ReferenceMapper& refs,                                                 \
        std::function<void(_writerType&)>* cb) const

// The implementation/body of a `OSSIACO_INTERNAL_TOJSON_METHOD_SIG` method.
#define OSSIACO_INTERNAL_TOJSON_METHOD_BODY(_type)                                                 \
    {                                                                                              \
        using namespace Ossiaco::converter;                                                        \
        JsonSerializer<_type>::template toJson(*this, writer, refs, cb);                           \
    }

// A virtual `toJson` method using `OSSIACO_INTERNAL_TOJSON_METHOD_SIG` and
// `OSSIACO_INTERNAL_TOJSON_METHOD_BODY`.
#define OSSIACO_INTERNAL_TOJSON_METHOD_VIRTUAL_IMPL(_writerType, _type)                            \
    virtual OSSIACO_INTERNAL_TOJSON_METHOD_SIG(_writerType) OSSIACO_INTERNAL_TOJSON_METHOD_BODY(_type)

// An override of a method created with `OSSIACO_INTERNAL_TOJSON_METHOD_VIRTUAL_IMPL`.
#define OSSIACO_INTERNAL_TOJSON_METHOD_OVERRIDE_IMPL(_writerType, _type)                           \
    OSSIACO_INTERNAL_TOJSON_METHOD_SIG(_writerType) override OSSIACO_INTERNAL_TOJSON_METHOD_BODY(_type)

// A template `toJson` method which delegates to the appropriate virtual or override `toJson` by writer type.
#define OSSIACO_INTERNAL_TOJSON_TEMPLATE_METHOD                                                    \
    template<typename Writer>                                                                      \
    void toJson(Writer& writer) const                                                              \
    {                                                                                              \
        Ossiaco::converter::ReferenceMapper rm;                                                    \
        this->toJson(writer, rm, nullptr);                                                         \
    }

// Pure template `toJson` methods for classes not using polymorphic JSON conversion.
#define OSSIACO_INTERNAL_PURE_TEMPLATE_TOJSON_METHODS(_type)                                       \
    template<typename Writer>                                                                      \
    void toJson(                                                                                   \
        Writer& writer,                                                                            \
        Ossiaco::converter::ReferenceMapper& refs,                                                 \
        std::function<void(Writer&)>* cb) const                                                    \
    {                                                                                              \
        Ossiaco::converter::JsonSerializer<_type>::template toJson(*this, writer, refs, cb);                \
    }                                                                                              \
    template<typename Writer>                                                                      \
    void toJson(Writer& writer) const                                                              \
    {                                                                                              \
        Ossiaco::converter::ReferenceMapper refs;                                                  \
        this->template toJson<Writer>(writer, refs, nullptr);                                                             \
    }

// Provide `_type` with definitions/implementations of toJson for all writer types, using `_implMacro`
// which should be OSSIACO_INTERNAL_TOJSON_METHOD_[OVERRIDE|VIRTUAL]_IMPL.
#define OSSIACO_INTERNAL_TOJSON_METHOD_IMPLS(_implMacro, _type)        \
OSSIACO_INTERNAL_TOJSON_TEMPLATE_METHOD                                \
_implMacro(Ossiaco::converter::FileStreamWriter, _type)                \
_implMacro(Ossiaco::converter::StringBufferWriter, _type)              \
_implMacro(Ossiaco::converter::InsituStringStreamWriter, _type)        \
_implMacro(Ossiaco::converter::PrettyFileStreamWriter, _type)          \
_implMacro(Ossiaco::converter::PrettyStringBufferWriter, _type)        \
_implMacro(Ossiaco::converter::PrettyInsituStringStreamWriter, _type)

// Friend declarations for converting _type, and an exposed tag type _tag for detection.
// Note that *all* type allocators are made a friend, because we cannot write partially specialized
// friend template declarations.
#define OSSIACO_INTERNAL_JSON_FRIENDS_AND_TAG(_type, _tag)                                         \
    friend class Ossiaco::converter::JsonConverter<_type>;                                         \
    template<typename A, typename B>                                                               \
    friend class Ossiaco::converter::TypeAllocator;                                                \
    using JsonConverterSupportTag = _tag;                                                          \
    static_assert(true, "Force trailing semicolon")

#if OSSIACO_MSVC_TUPLE_WORKAROUND
#    define OSSIACO_JSON_PROPERTIES_IMPL(_chainedCall)                                             \
        static constexpr auto& jsonProperties()                                                    \
        {                                                                                          \
            using namespace Ossiaco::converter;                                                    \
            static constexpr auto result = PropertiesHelper<> {}                                   \
            _chainedCall;                                                                          \
            return result._props;                                                                  \
        }
#else
// Provides a tuple of JSON properties by delegating to the chained call operator of PropertiesHelper.
// This implementation cannot be used with MSVC due to a bug:
// https://developercommunity.visualstudio.com/content/problem/202891/visual-studio-1560-preview-60-breaks-existing-cons.html
// The MSVC workaround implementaiton above could be used unilaterally but for the fact that GCC does not allow
// static variables to be defined in constexpr functions.
#    define OSSIACO_JSON_PROPERTIES_IMPL(_chainedCall)                                             \
        static constexpr auto jsonProperties()                                                     \
        {                                                                                          \
            using namespace Ossiaco::converter;                                                    \
            return (PropertiesHelper<> {} _chainedCall)._props;                                    \
        }
#endif

// Define and implement the method returning a tuple of JSON properties.
// TODO the return type is auto& due to an MSVC constexpr bug.

#define OSSIACO_INTERNAL_JSON_TYPE_ENUM(_enum) using JsonEnumType = _enum;

#define OSSIACO_INTERNAL_JSON_BASE(_type) using JsonBase = _type;

/// Put this macro in public visibility of `_type` to support non-polymorphic JSON conversion.
#define OSSIACO_CONVERTER_FINAL_SUPPORTED(_type, _propsCall)                                       \
    OSSIACO_JSON_PROPERTIES_IMPL(_propsCall)                                              \
    OSSIACO_INTERNAL_PURE_TEMPLATE_TOJSON_METHODS(_type)                                           \
    OSSIACO_INTERNAL_JSON_FRIENDS_AND_TAG(_type, Ossiaco::converter::traits::FinalSupportTag)

/// Put this macro in public visibility in `_type` that shall be base class in a hierarchy supporting JSON
/// conversion.
#define OSSIACO_CONVERTER_BASE_SUPPORTED(_type, _propsCall)                                        \
    OSSIACO_JSON_PROPERTIES_IMPL(_propsCall)                                              \
    OSSIACO_INTERNAL_TOJSON_METHOD_IMPLS(OSSIACO_INTERNAL_TOJSON_METHOD_VIRTUAL_IMPL, _type)       \
    OSSIACO_INTERNAL_JSON_FRIENDS_AND_TAG(_type, Ossiaco::converter::traits::PolySupportTag)

/// Put this macro in public visibility in `_type` that shall be base class in a hierarchy supporting JSON
/// conversion through the enum `_enum`.
#define OSSIACO_CONVERTER_BASE_MAPPED_SUPPORTED(_type, _enum, _propsCall)                          \
    OSSIACO_INTERNAL_JSON_TYPE_ENUM(_enum)                                                         \
    OSSIACO_CONVERTER_BASE_SUPPORTED(_type, _propsCall)

/// Put this macro in public visibility in `_type` that shall be polymorphically convertible via
/// `_baseType`.
#define OSSIACO_CONVERTER_POLY_SUPPORTED(_type, _baseType, _propsCall)                             \
    OSSIACO_JSON_PROPERTIES_IMPL(_propsCall)                                              \
    OSSIACO_INTERNAL_JSON_BASE(_baseType)                                                          \
    OSSIACO_INTERNAL_TOJSON_METHOD_IMPLS(OSSIACO_INTERNAL_TOJSON_METHOD_OVERRIDE_IMPL, _type)      \
    OSSIACO_INTERNAL_JSON_FRIENDS_AND_TAG(_type, Ossiaco::converter::traits::PolySupportTag)

#endif // OSSIACO_CONVERTER_ADAPT_HPP
