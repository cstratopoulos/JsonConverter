// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

// Tests of unilateral conversion where we can do serialization only.

#include "../test_utils.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/components/ranges.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <catch2/catch.hpp>

#include <vector>

#define _T(X) OSSIACO_XPLATSTR(X)

namespace Oc = Ossiaco::converter;
using Oc::string_t;

using namespace boost::mp11;

struct PublicData {
	int _i;
	string_t _s;

	OSSIACO_JSON_PROPERTIES_IMPL(
		(&PublicData::_i, _T("i"))
		(&PublicData::_s, _T("s"))
	);
};

class PrivateDataRefGetters {
public:
	PrivateDataRefGetters(string_t s, std::vector<double> v)
		: _s(std::move(s)), _v(std::move(v))
	{}

	const string_t& s() const { return _s; }

	const std::vector<double>& v() const { return _v; }

	OSSIACO_JSON_PROPERTIES_IMPL(
		(&PrivateDataRefGetters::s, _T("s"))
		(&PrivateDataRefGetters::v, _T("v"))
	);

private:
	string_t _s;

	std::vector<double> _v;

};

class PrivateDataValGetters {
public:
	PrivateDataValGetters(int i, double d) : _i(i), _d(d) {}

	int i() const { return _i; }
	double d() const { return _d; }

	OSSIACO_JSON_PROPERTIES_IMPL(
		(&PrivateDataValGetters::i, _T("i"))
		(&PrivateDataValGetters::d, _T("d"))
	);

private:
	int _i;
	double _d;
};

class MixedGetterPublic {
public:

	MixedGetterPublic(int i, int j, std::vector<double> v)
		: _i(i), _j(j), _v(std::move(v))
	{}

	int _i;

	int j() const { return _j; }
	const std::vector<double>& v() const { return _v; }

	OSSIACO_JSON_PROPERTIES_IMPL(
		(&MixedGetterPublic::_i, _T("i"))
		(&MixedGetterPublic::j, _T("j"))
		(&MixedGetterPublic::v, _T("v"))
	);
private:
	int _j;
	std::vector<double> _v;
};

class ChronoFmtGetter {
public:
	ChronoFmtGetter(date::year_month_day d) : _d(d) {}

	date::year_month_day d() const { return _d; }

	OSSIACO_JSON_PROPERTIES_IMPL(
		(Oc::chronoFmtPair(&ChronoFmtGetter::d, _T("%Y-%m-%d")), _T("d"))
	);
private:
	date::year_month_day _d;
};

using TestTypes = mp_list<
	PublicData,
	PrivateDataRefGetters,
	PrivateDataValGetters,
	MixedGetterPublic,
	ChronoFmtGetter
>;

template<typename JsonProp>
using PropertyHasToJson = mp_bool<JsonProp::enableToJson>;

template<typename JsonProp>
using PropertyHasFromJson = mp_bool<JsonProp::enableFromJson>;

// Checks if all properties in T's jsonProperties tuple have toJson.
template<typename T>
using AllPropertiesHaveToJson = mp_all_of<
	Oc::traits::JsonPropertiesType<T>,
	PropertyHasToJson
>;

template<typename T>
using JsonSupportDetected = mp_bool<Oc::traits::jsonSupportDetected<T>>;

TEST_CASE("Unilateral serialization", "[OSSIACO_JSON_PROPERTIES_IMPL]")
{
    SECTION("JsonProperty checking")
    {
        STATIC_REQUIRE(mp_all_of<TestTypes, AllPropertiesHaveToJson>());

		STATIC_REQUIRE(mp_none_of<TestTypes, JsonSupportDetected>());
    }

    SECTION("Unilateral serialization string appearance")
    {
        jsonCompare(Oc::toJsonStringPretty(PublicData{1, _T("dog")}), _T(R"--({
"i": 1,
"s": "dog"
})--"));

        jsonCompare(
            Oc::toJsonStringPretty(PrivateDataRefGetters(_T("cat"), {1.0, 2.0, 3.0})), _T(R"--({
"s": "cat",
"v": [1.0, 2.0, 3.0]
})--"));

        jsonCompare(
            Oc::toJsonStringPretty(PrivateDataValGetters(5, 26.3)),
            _T(R"--({"i": 5, "d": 26.3})--"));

		jsonCompare(
			Oc::toJsonStringPretty(MixedGetterPublic(1, 2, { 3.0, 4.0 })),
			_T(R"--({
"i": 1,
"j": 2,
"v": [3.0, 4.0]
})--"));

		using namespace date::literals;

		jsonCompare(
			Oc::toJsonStringPretty(ChronoFmtGetter(2011_y / date::nov / 11)),
			_T(R"--({"d": "2011-11-11"})--"));
    }
}
