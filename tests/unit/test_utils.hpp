// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TEST_UTILS_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TEST_UTILS_HPP

#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/buffers_streams_writers.hpp>
#include <ossiaco/converter/serialize.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>
#include <ossiaco/converter/utils/print_type_name.hpp>
#include <ossiaco/converter/utils/string_convert.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/tuple.hpp>
#include <boost/type_traits/remove_cv_ref.hpp>
#include <range/v3/utility/concepts.hpp>

#include <catch.hpp>

#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>

#if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif __has_include(<filesystem>)
#    include <filesystem>
namespace fs = std::filesystem;
#else
#    error "Filesystem implementation needed for unit tests"
#endif

// Class providing interface to check correct rehydration of objects, either by
// equality comparison or equality comparison of selected getters
template<typename Class, typename... Args>
class ObjectComparison;

// Type deduction helper for ObjectComparison
template<typename Class, typename... Args>
auto makeObjectComparison(Args... args)
{
    return ObjectComparison<Class, Args...>{args...};
}

struct StringObjectConversion {
    template<typename Derived, typename Base = Derived>
    static std::shared_ptr<Base> testConvert(const Derived& obj);
};

struct FileObjectConversion {
    explicit FileObjectConversion(fs::path path) : _path(path)
    {
        if (_path.extension().string<Ossiaco::converter::CharType>() != OSSIACO_XPLATSTR(".json"))
            throw std::logic_error(
                "FileObjectConversion must be constructed with path to .json file");

        fs::create_directories(_path.parent_path());
    }

    explicit FileObjectConversion(const Ossiaco::converter::CharType* pathName)
        : FileObjectConversion(fs::path(pathName))
    {}

    template<typename Derived, typename Base = Derived>
    std::shared_ptr<Base> testConvert(const Derived& obj) const;

    const fs::path _path;
};

template<typename Base, typename Func, typename Comparison, typename Conversion>
struct BasicTestCase {
    static_assert(
        std::is_invocable_r_v<const Base&, Func>,
        "The constructor function of a BasicTestCase must return base or an inheritor");

    void run();

    std::string_view _desc;

    const Func _ctorFunc;

    const Comparison _comparator;

    const Conversion _converter;
};

// Template deduction helper for constructing a BasicTestCase
template<typename Base, typename Func, typename Comparison, typename Conversion>
auto makeBasicTestCase(std::string_view desc, Func&& f, Comparison&& comp, Conversion&& conv)
{
    return BasicTestCase<Base, Func, Comparison, Conversion>{
        desc, std::forward<Func>(f), std::forward<Comparison>(comp), std::forward<Conversion>(conv)};
}

// Template deduction helper for non-polymorphic BasicTestCase using string conversion
template<typename Func, typename... Args>
auto makeSimpleStringTest(std::string_view desc, Func&& f, Args... args)
{
    using Base = std::invoke_result_t<Func>;

    return makeBasicTestCase<Base>(
        desc, std::forward<Func>(f), makeObjectComparison<Base>(args...), StringObjectConversion{});
}

// Template deduction helper for non-polymorphic BasicTestCase using file conversion
template<typename Func, typename... Args>
auto makeSimpleFileTest(
    std::string_view desc, Func&& f, const Ossiaco::converter::CharType* path, Args... args)
{
    using Base = std::invoke_result_t<Func>;

    return makeBasicTestCase<Base>(
        desc,
        std::forward<Func>(f),
        makeObjectComparison<Base>(args...),
        FileObjectConversion(path));
}

template<typename... TestCases>
void runTestCases(TestCases&&... tcs)
{
    (tcs.run(), ...);
}

template<typename Derived, typename Base>
std::shared_ptr<Base> StringObjectConversion::testConvert(const Derived& obj)
{
    namespace oc = Ossiaco::converter;

    oc::string_t objString;

    REQUIRE_NOTHROW([&]() mutable { objString = oc::toJsonStringPretty(obj); }());
    CAPTURE(objString);

    std::shared_ptr<Base> newObj{};

    REQUIRE_NOTHROW(
        [&newObj, &objString] { newObj = oc::JsonDeserializer<Base>::fromString(objString); }());

    return newObj;
}

template<typename Derived, typename Base>
std::shared_ptr<Base> FileObjectConversion::testConvert(const Derived & obj) const
{
    namespace oc = Ossiaco::converter;

    REQUIRE_NOTHROW(oc::toJsonFilePretty(obj, _path.c_str()));

    std::shared_ptr<Base> newObj{};

    REQUIRE_NOTHROW(
        [&newObj, this] { newObj = oc::JsonDeserializer<Base>::fromJsonFile(_path.c_str()); }());

    return newObj;
}

template<typename Class>
class ObjectComparison<Class> {
public:
    static_assert(ranges::EqualityComparable<Class>(),
        "Object comparison with no member function pointers requires EqualityComparable");

    template<typename Derived>
    static void compareObjects(const Derived& origObj, const Derived& newObj)
    {
        static_assert(std::is_base_of_v<Class, Derived>);
        CHECK(origObj == newObj);
    }
};

template<typename Class, typename Arg, typename... Args>
class ObjectComparison<Class, Arg, Args...> {
public:
    using TupleType = std::tuple<Arg, Args...>;

    static_assert(!ranges::EqualityComparable<Class>(),
        "Object comparison with member function pointers is not allowed for EqualityComparable");

    static_assert(boost::mp11::mp_all_of<TupleType, std::is_member_function_pointer>());

    ObjectComparison(Arg arg, Args... args) : _pmfs{ arg, args... } {}

    template<typename Derived>
    void compareObjects(const Derived& origObj, const Derived& newObj) const
    {
        static_assert(std::is_base_of_v<Class, Derived>);

        boost::mp11::tuple_for_each(_pmfs, [&](const auto methodPtr) {
            namespace oc = Ossiaco::converter;
            INFO("Testing method pointer: " << oc::toNarrowString(oc::printTypeName<decltype(methodPtr)>));

            const auto&[origRet, newRet] =
                std::make_pair((origObj.*methodPtr)(), (newObj.*methodPtr)());

            using OrigRet = std::invoke_result_t<decltype(methodPtr), Derived>;
            using RetType = boost::remove_cv_ref_t<OrigRet>;

            if constexpr (
                oc::isSpecialization<RetType, std::weak_ptr> ||
                oc::isSpecialization<RetType, std::shared_ptr>) {
                CHECK(static_cast<bool>(origRet) == static_cast<bool>(newRet));

                if (origRet)
                    CHECK(*origRet == *newRet);

            } else {
                CHECK(origRet == newRet);
            }
        });
    }

private:
    const TupleType _pmfs;
};

template<typename Base, typename Func, typename Comparison, typename Conversion>
void BasicTestCase<Base, Func, Comparison, Conversion>::run()
{
    GIVEN(std::string(_desc))
    {
        auto obj = _ctorFunc();

        using ObjType = decltype(obj);
        using ObjConverter = Ossiaco::converter::JsonConverter<ObjType>;

        THEN("We should be able to hydrate/rehydrate it correctly") {
            std::shared_ptr<ObjType> newObj = _converter.testConvert(obj);
            REQUIRE(newObj);

            _comparator.compareObjects(obj, *newObj);

            if constexpr (!std::is_same_v<ObjType, Base>) {
                AND_THEN("Polymorphic objects should be registered") {
                    static_assert(ObjConverter::_isPolymorphic);
                    REQUIRE(Ossiaco::converter::jsonPolyImpl<ObjType>());

                    AND_THEN("We should be able to polymorphically rehydrate it correctly") {
                        std::shared_ptr<Base> newObjBase = _converter.template testConvert<Base>(obj);

                        ObjType* downCastPtr = dynamic_cast<ObjType*>(newObjBase.get());
                        REQUIRE(downCastPtr);
                        _comparator.compareObjects(obj, *downCastPtr);
                    }
                }
            }
        }
    }
}

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TEST_UTILS_HPP