// cs/net/json/absurd_json_test.gpt.cc
#include <string>
#include <vector>

#include "cs/net/json/json_dsl.gpt.hh"

namespace {  // use_usings
using ::cs::net::json::Object;
using ::cs::net::json::dsl::AsNumber;
using ::cs::net::json::dsl::Case;
using ::cs::net::json::dsl::CaseRegistrar;
using ::cs::net::json::dsl::Registry;
using ::cs::net::json::dsl::RunAll;
}  // namespace

// Focused scalar sanity checks (no nulls because parser
// lacks them).
JSON_OK(SCALAR_TRUE, "true", J_BOOL(true),
        "RFC 8259 Section 3 true");
JSON_OK(SCALAR_FALSE, "false", J_BOOL(false),
        "RFC 8259 Section 3 false");
JSON_OK(SCALAR_INT_ZERO, "0", J_INT(0), "Section 6 int");
JSON_OK(SCALAR_INT_NEG, "-123", J_INT(-123),
        "Section 6 int");
JSON_OK(SCALAR_FLOAT_SIMPLE, "3.14", J_FLOAT(3.14f),
        "Section 6 fraction");
JSON_OK(SCALAR_EXP_POS, "1e3", J_FLOAT(1000.0f),
        "Section 6 exponent");
JSON_OK(SCALAR_EXP_NEG, "5E-2", J_FLOAT(0.05f),
        "Section 6 exponent");
JSON_OK(SCALAR_STRING_BASIC, "\"json\"", J_STR("json"),
        "Section 7 string");
JSON_OK(SCALAR_STRING_ESC_QUOTE, "\"\\\"\"", J_STR("\""),
        "Section 7 escape quote");
JSON_FAIL(SCALAR_STRING_RAW_LF, "\"a\nb\"",
          "Section 7 control chars must be escaped");

// Object/array structure primers.
JSON_OK(ARRAY_EMPTY, "[]", J_ARR(),
        "Section 5 array empty");
JSON_OK(ARRAY_BOOL, "[true,false,true]",
        J_ARR(J_BOOL(true), J_BOOL(false), J_BOOL(true)),
        "Section 5 array of bools");
JSON_OK(ARRAY_NUM, "[1,2,3,4]",
        J_ARR(J_INT(1), J_INT(2), J_INT(3), J_INT(4)),
        "Section 5 numeric array");
JSON_OK(ARRAY_NEST, "[1,[2,[3,[4]]]]",
        J_ARR(J_INT(1),
              J_ARR(J_INT(2),
                    J_ARR(J_INT(3), J_ARR(J_INT(4))))),
        "Section 5 nested arrays");
JSON_OK(ARRAY_MIX, "[\"x\",1.5,false,{\"k\":7},[]]",
        J_ARR(J_STR("x"), J_FLOAT(1.5f), J_BOOL(false),
              J_OBJ(J_KV("k", J_INT(7))), J_ARR()),
        "Section 3-Section 5 mixed array");

JSON_OK(OBJ_EMPTY, "{}", J_OBJ(), "Section 4 empty object");
JSON_OK(OBJ_SIMPLE, "{\"a\":1}", J_OBJ(J_KV("a", J_INT(1))),
        "Section 4 single member");
JSON_OK(OBJ_MULTI, "{\"a\":1,\"b\":true,\"c\":\"x\"}",
        J_OBJ(J_KV("a", J_INT(1)), J_KV("b", J_BOOL(true)),
              J_KV("c", J_STR("x"))),
        "Section 4 multi member");
JSON_OK(OBJ_NEST, "{\"a\":{\"b\":[1,2],\"c\":{\"d\":3}}}",
        J_OBJ(J_KV(
            "a",
            J_OBJ(J_KV("b", J_ARR(J_INT(1), J_INT(2))),
                  J_KV("c", J_OBJ(J_KV("d", J_INT(3))))))),
        "Section 3-Section 5 nested map/array");
JSON_OK(OBJ_WS, " { \"spaced\" : [ 1 , 2 , 3 ] } ",
        J_OBJ(J_KV("spaced",
                   J_ARR(J_INT(1), J_INT(2), J_INT(3)))),
        "Section 2 whitespace");

// Invalid per RFC; parser should reject these.
JSON_FAIL(INVALID_EMPTY, "",
          "Section 2 missing value should fail");
JSON_FAIL(INVALID_HASH, "#",
          "Section 2 illegal leading character");
JSON_FAIL(INVALID_TRAIL_COMMA_ARR, "[1,2,]",
          "Section 2 trailing comma array");
JSON_FAIL(INVALID_TRAIL_COMMA_OBJ, "{\"a\":1,}",
          "Section 2 trailing comma object");
JSON_FAIL(INVALID_UNTERM_OBJ, "{\"a\":1",
          "Section 4 missing closing brace");
JSON_FAIL(INVALID_UNTERM_ARR, "[1,2",
          "Section 5 missing closing bracket");
JSON_FAIL(INVALID_NO_COLON, "{\"a\" 1}",
          "Section 4 missing colon");
JSON_FAIL(INVALID_DOUBLE_COMMA, "[1,,2]",
          "Section 5 double comma");
JSON_FAIL(INVALID_LEADING_ZERO, "01",
          "Section 6 leading zero");
JSON_FAIL(INVALID_PLUS_ONLY, "+",
          "Section 6 sign without digits");
JSON_FAIL(INVALID_EXP_EMPTY, "1e",
          "Section 6 exponent digits required");
JSON_FAIL(INVALID_TRAILING_DOT, "9.",
          "Section 6 digits required after decimal");
JSON_FAIL(INVALID_LEADING_DOT, ".5",
          "Section 6 digit required before decimal");
JSON_FAIL(INVALID_COMMENT_SLASH, "// hi",
          "Section 2 comments not allowed");
JSON_FAIL(INVALID_SINGLE_QUOTE, "{'a':1}",
          "Section 7 strings must be double quoted");
JSON_OK(VALID_UNICODE_ESCAPE, "\"\\u0041\"", J_STR("A"),
        "Section 7 unicode escape");
JSON_OK(VALID_UNICODE_2BYTE, "\"\\u07FF\"",
        J_STR(std::string("\xDF\xBF")),
        "Section 7 unicode 2-byte");
JSON_OK(VALID_UNICODE_3BYTE, "\"\\u20AC\"",
        J_STR(std::string("\xE2\x82\xAC")),
        "Section 7 unicode 3-byte");
JSON_OK(VALID_UNICODE_4BYTE, "\"\\uD83D\\uDE00\"",
        J_STR(std::string("\xF0\x9F\x98\x80")),
        "Section 7 unicode surrogate pair");
JSON_FAIL(INVALID_UNICODE_HEX, "\"\\uZZZZ\"",
          "Section 7 invalid hex");
JSON_FAIL(INVALID_UNICODE_SURROGATE, "\"\\uD800x\"",
          "Section 7 invalid surrogate");
JSON_FAIL(INVALID_UNICODE_LOW_SURROGATE, "\"\\uDC00\"",
          "Section 7 unexpected low surrogate");
JSON_FAIL(INVALID_PLUS_NUMBER, "+7",
          "Section 6 leading plus not allowed");
JSON_FAIL(INVALID_TRAIL_DOT_FLOAT, "10.",
          "Section 6 digits required after decimal");
JSON_FAIL(INVALID_LEAD_DOT_FLOAT, ".125",
          "Section 6 digit required before decimal");
JSON_FAIL(
    INVALID_PLUS_LEAD_DOT_FLOAT, "+.5",
    "Section 6 leading plus and missing leading digit");
JSON_FAIL(INVALID_PLUS_EXP_FLOAT, "+2e2",
          "Section 6 leading plus not allowed");
JSON_FAIL(INVALID_MINUS_DOT_FLOAT, "-.0",
          "Section 6 digit required before decimal");

// JSON DSL xfail coverage cases (intentional mismatches).
JSON_OK_XFAIL(MISMATCH_NUMERIC, "1", J_FLOAT(2.0f),
              "dsl numeric mismatch");
JSON_OK_XFAIL(MISMATCH_TYPE, "true", J_STR("true"),
              "dsl type mismatch");
JSON_OK_XFAIL(MISMATCH_ARRAY_SIZE, "[1,2]", J_ARR(J_INT(1)),
              "dsl array size mismatch");
JSON_OK_XFAIL(MISMATCH_MAP_KEY, "{\"a\":1}",
              J_OBJ(J_KV("b", J_INT(1))),
              "dsl map missing key");
JSON_OK_XFAIL(MISMATCH_ARRAY_VALUE, "[1,2]",
              J_ARR(J_INT(1), J_INT(3)),
              "dsl array element mismatch");
JSON_OK_XFAIL(MISMATCH_MAP_SIZE, "{\"a\":1,\"b\":2}",
              J_OBJ(J_KV("a", J_INT(1))),
              "dsl map size mismatch");
JSON_OK_XFAIL(MISMATCH_MAP_VALUE, "{\"a\":1}",
              J_OBJ(J_KV("a", J_INT(2))),
              "dsl map value mismatch");
JSON_OK_XFAIL(PARSE_FAIL_EXPECTED_OK, "{", J_NULL(),
              "dsl parse failure under ok");
JSON_OK_XFAIL(XFAIL_NOW_PASSING, "1", J_INT(1),
              "dsl xfail now passing");
JSON_FAIL_XFAIL(UNEXPECTED_SUCCESS, "true",
                "dsl expected parse error");

[[maybe_unused]]
static const bool kCoverAsNumberNonNumeric = []() {
  auto value = AsNumber(J_STR("not-a-number"));
  (void)value;
  return true;
}();

// Bulk integer sweep (-50..50 step 5, with and without
// leading '+').
[[maybe_unused]]
static const bool kRegisterIntSweep = []() {
  for (int v = -50; v <= 50; v += 5) {
    Registry().push_back(
        Case{"INT_SWEEP_" + std::to_string(v),
             std::to_string(v), J_INT(v),
             /*should_parse=*/true,
             /*check_roundtrip=*/true,
             "Section 6 integer sweep", /*xfail=*/false});
  }
  for (int v = 0; v <= 50; v += 5) {
    Registry().push_back(
        Case{"INT_PLUS_INVALID_" + std::to_string(v),
             "+" + std::to_string(v), J_INT(v),
             /*should_parse=*/false,
             /*check_roundtrip=*/false,
             "Section 6 leading plus not allowed",
             /*xfail=*/false});
  }
  return true;
}();

// Bulk float/exponent sweep.
[[maybe_unused]]
static const bool kRegisterFloatSweep = []() {
  struct Spec {
    const char* id;
    const char* text;
    float value;
    bool roundtrip;
  };
  const std::vector<Spec> specs = {
      {"F_SIMPLE_0_1", "0.1", 0.1f, true},
      {"F_SIMPLE_1_5", "1.5", 1.5f, true},
      {"F_SIMPLE_NEG", "-2.75", -2.75f, true},
      {"F_BIG_EXP", "1.23e4", 12300.0f, true},
      {"F_NEG_EXP", "9.9e-3", 0.0099f, true},
      {"F_POS_EXP_SIGN", "6E+2", 600.0f, true},
      {"F_NEG_EXP_SIGN", "-7E-1", -0.7f, true},
      {"F_SMALL", "0.0001", 0.0001f, true},
      {"F_LONG_FRAC", "3.141592", 3.141592f, true},
      {"F_EXP_ZERO", "4e0", 4.0f, true},
      {"F_EXP_LARGE", "1e6", 1'000'000.0f, true},
      {"F_EXP_TINY", "5e-5", 0.00005f, true},
      {"F_EXP_PLUS_ZERO", "8e+0", 8.0f, true},
      {"F_EXP_NEG_NO_INT", "-.5e2", -50.0f, true},
  };
  for (const auto& s : specs) {
    Registry().push_back(
        Case{s.id, s.text, J_FLOAT(s.value),
             /*should_parse=*/true,
             /*check_roundtrip=*/s.roundtrip,
             "Section 6 float/exponent sweep",
             /*xfail=*/false});
  }
  return true;
}();

// Array sweeps of ascending ints.
[[maybe_unused]]
static const bool kRegisterArraySweep = []() {
  for (int len = 0; len <= 24; ++len) {
    std::string json = "[";
    ::cs::net::json::Object::Array arr;
    for (int i = 0; i < len; ++i) {
      if (i > 0) json.push_back(',');
      json += std::to_string(i);
      arr.push_back(J_INT(i));
    }
    json.push_back(']');
    Registry().push_back(
        Case{"ARR_SEQ_" + std::to_string(len), json,
             ::cs::net::json::Object::NewArray(arr),
             /*should_parse=*/true,
             /*check_roundtrip=*/true,
             "Section 5 array length sweep",
             /*xfail=*/false});
  }
  return true;
}();

// Arrays mixing booleans and strings.
[[maybe_unused]]
static const bool kRegisterArrayMix = []() {
  const std::vector<std::string> words = {
      "alpha", "beta", "gamma", "delta", "epsilon"};
  for (size_t i = 0; i < words.size(); ++i) {
    std::string json = "[";
    ::cs::net::json::Object::Array arr;
    for (size_t j = 0; j <= i; ++j) {
      if (j > 0) json.push_back(',');
      if (j % 2 == 0) {
        json.push_back('"');
        json += words[j];
        json.push_back('"');
        arr.push_back(J_STR(words[j]));
      } else {
        if (j % 4 == 1) {
          json += "true";
        } else {
          json += "false";
        }
        arr.push_back(J_BOOL(j % 4 == 1));
      }
    }
    json.push_back(']');
    Registry().push_back(
        Case{"ARR_MIX_" + std::to_string(i), json,
             ::cs::net::json::Object::NewArray(arr),
             /*should_parse=*/true,
             /*check_roundtrip=*/true,
             "Section 3-Section 5 mixed array sweep",
             /*xfail=*/false});
  }
  return true;
}();

// Object sweeps with ascending keys.
[[maybe_unused]]
static const bool kRegisterObjectSweep = []() {
  for (int n = 1; n <= 12; ++n) {
    std::string json = "{";
    ::cs::net::json::Object::KVMap map;
    for (int i = 0; i < n; ++i) {
      if (i > 0) json.push_back(',');
      std::string key = "k" + std::to_string(i);
      json.push_back('"');
      json += key;
      json.push_back('"');
      json += ":";
      json += std::to_string(i);
      map[key] = J_INT(i);
    }
    json.push_back('}');
    Registry().push_back(
        Case{"OBJ_SEQ_" + std::to_string(n), json,
             ::cs::net::json::Object(map),
             /*should_parse=*/true,
             /*check_roundtrip=*/true,
             "Section 4 object field sweep",
             /*xfail=*/false});
  }
  return true;
}();

// Deeply nested object+array shapes.
JSON_OK(DEEP_MIXED, "{\"a\":[{\"b\":[{\"c\":[1,2,3]}]}]}",
        J_OBJ(J_KV(
            "a", J_ARR(J_OBJ(J_KV(
                     "b", J_ARR(J_OBJ(J_KV(
                              "c", J_ARR(J_INT(1), J_INT(2),
                                         J_INT(3)))))))))),
        "Section 3-Section 5 deep mixed nest");
JSON_OK(OBJ_WITH_EMPTY, "{\"a\":{},\"b\":[]}",
        J_OBJ(J_KV("a", J_OBJ()), J_KV("b", J_ARR())),
        "Section 4 & Section 5 empty members");

// Escape/unicode should decode.
JSON_OK(STRING_UNICODE, "\"\\u263A\"",
        J_STR("\xE2\x98\xBA"), "Section 7 unicode escape");
JSON_OK(STRING_SURROGATE, "\"\\uD83D\\uDE00\"",
        J_STR("\xF0\x9F\x98\x80"),
        "Section 7 surrogate pair");
JSON_OK(STRING_BACKSPACE, "\"a\\bb\"", J_STR("a\bb"),
        "Section 7 backspace");
JSON_OK(STRING_TAB, "\"a\\tb\"", J_STR("a\tb"),
        "Section 7 tab");
JSON_OK(STRING_FORMFEED, "\"a\\fb\"", J_STR("a\fb"),
        "Section 7 formfeed");

// Null-focused expectations: should parse.
JSON_OK(NULL_TOP, "null", J_NULL(),
        "Section 3 null literal");
JSON_OK(NULL_IN_OBJ, "{\"n\":null}",
        J_OBJ(J_KV("n", J_NULL())),
        "Section 3 null in object");
JSON_OK(NULL_IN_ARR, "[1,null,2]",
        J_ARR(J_INT(1), J_NULL(), J_INT(2)),
        "Section 3 null in array");

int main() { return RunAll(std::cout); }
