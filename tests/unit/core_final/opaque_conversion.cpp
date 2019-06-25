#include "../test_utils.hpp"
#include "../types/opaque.hpp"

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>

#include <catch2/catch.hpp>

namespace Oc = Ossiaco::converter;

namespace tt = test_types;

struct HoldsOpaque {
    Oc::string_t s;
    tt::Opaque o;

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        HoldsOpaque, (&HoldsOpaque::s, TSTR("s"))(&HoldsOpaque::o, TSTR("o")));
};

TEST_CASE("First class conversion of Opaque struct", "[ConvertFirstClass]")
{
    tt::Opaque o;
    o.i = 123;
    o.d = 45.6;

    Oc::DefaultStringBuffer sb;
    Oc::PrettyStringBufferWriter w(sb);

    o.toJson(w);

    jsonCompare(sb.GetString(), TSTR(R"--(
{
	"i": 123,
	"d": 45.6
}
)--"));
}

TEST_CASE("Conversion with opaquely adapted struct", "[DeducedConverter]")
{
    HoldsOpaque h;
    tt::Opaque o;

    o.i = 123;
    o.d = 45.6;
    h.s = TSTR("dog");
    h.o = o;

    const Oc::string_view_t jsonStr = TSTR(R"--(
	{
		"s": "dog",
		"o":
			{
				"i": 123,
				"d": 45.6
			}
	}
)--");

    jsonCompare(Oc::toJsonStringPretty(h), jsonStr);

    auto other = Oc::JsonDeserializer<HoldsOpaque>::fromString(jsonStr);

    CHECK(other->s == h.s);
    CHECK(other->o.i == o.i);
    CHECK(other->o.d == o.d);
}
