// cs/net/rpc/extract_proto_from_request_test.gpt.cc
#include <map>
#include <string>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/rpc/protos/extract_test.proto.hh"
#include "cs/net/rpc/rpc.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::kContentTypeXWwwFormUrlEncoded;
using ::cs::net::http::Request;
using ::cs::net::rpc::protos::ExtractTestProto;
}  // namespace

namespace cs::net::rpc {
namespace {

using ::testing::IsFalse;
using ::testing::IsTrue;
using Proto = ExtractTestProto;

Request MakeRequest(
    std::string method,
    std::map<std::string, std::string> query_params,
    std::map<std::string, std::string> headers,
    std::string body) {
  Request req;
  req._method = std::move(method);
  req._path = "/";
  req._query_params = std::move(query_params);
  req._headers = std::move(headers);
  req._body = std::move(body);
  return req;
}

// GET: no query params -> "{}"
TEST(ExtractProtoFromRequestTest, GetNoQueryParams) {
  auto req = MakeRequest("GET", {}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "");
  EXPECT_EQ(result.value().count, 0);
}

// GET: one query param, string value
TEST(ExtractProtoFromRequestTest, GetOneQueryParamString) {
  auto req = MakeRequest("GET", {{"name", "foo"}}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "foo");
  EXPECT_EQ(result.value().count, 0);
}

// GET: one query param, numeric string (QueryParamsToJson
// uses ParseInt)
TEST(ExtractProtoFromRequestTest, GetOneQueryParamNumeric) {
  auto req = MakeRequest("GET", {{"count", "42"}}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "");
  EXPECT_EQ(result.value().count, 42);
}

// GET: query param value "0" produces JSON number 0
TEST(ExtractProtoFromRequestTest, GetQueryParamZero) {
  auto req = MakeRequest("GET", {{"count", "0"}}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().count, 0);
}

// GET: multiple query params (string and int)
TEST(ExtractProtoFromRequestTest, GetMultipleQueryParams) {
  auto req = MakeRequest(
      "GET", {{"name", "bar"}, {"count", "7"}}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "bar");
  EXPECT_EQ(result.value().count, 7);
}

// POST: no Content-Type, empty body -> default
// form-urlencoded, "{}"
TEST(ExtractProtoFromRequestTest,
     PostNoContentTypeEmptyBody) {
  auto req = MakeRequest("POST", {}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "");
  EXPECT_EQ(result.value().count, 0);
}

// POST: application/x-www-form-urlencoded, body a=b&c=d
TEST(ExtractProtoFromRequestTest, PostFormUrlEncoded) {
  auto req = MakeRequest("POST", {}, {}, "a=b&c=d");
  req._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "");
  EXPECT_EQ(result.value().count, 0);
  // Form has a and c; our proto has name and count. So we
  // get defaults. Test with name and count in form.
  auto req2 =
      MakeRequest("POST", {}, {}, "name=foo&count=10");
  req2._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  auto result2 = ExtractProtoFromRequest<Proto>(req2);
  ASSERT_THAT(result2.ok(), IsTrue());
  EXPECT_EQ(result2.value().name, "foo");
  EXPECT_EQ(result2.value().count, 10);
}

// POST: form body with URL encoding
TEST(ExtractProtoFromRequestTest,
     PostFormUrlEncodedWithEncoding) {
  auto req = MakeRequest("POST", {}, {},
                         "name=hello%20world&count=1");
  req._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "hello world");
  EXPECT_EQ(result.value().count, 1);
}

// POST: application/json body
TEST(ExtractProtoFromRequestTest, PostApplicationJson) {
  auto req = MakeRequest("POST", {}, {},
                         R"({"name":"x","count":1})");
  req._headers["Content-Type"] =
      kContentTypeApplicationJson;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "x");
  EXPECT_EQ(result.value().count, 1);
}

// POST: application/json, empty body -> Parse receives ""
TEST(ExtractProtoFromRequestTest,
     PostApplicationJsonEmptyBody) {
  auto req = MakeRequest("POST", {}, {}, "");
  req._headers["Content-Type"] =
      kContentTypeApplicationJson;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsFalse());
}

// PUT: application/json (same as POST)
TEST(ExtractProtoFromRequestTest, PutApplicationJson) {
  auto req = MakeRequest("PUT", {}, {},
                         R"({"name":"put","count":2})");
  req._headers["Content-Type"] =
      kContentTypeApplicationJson;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "put");
  EXPECT_EQ(result.value().count, 2);
}

// PATCH: form-urlencoded (same as POST)
TEST(ExtractProtoFromRequestTest, PatchFormUrlEncoded) {
  auto req =
      MakeRequest("PATCH", {}, {}, "name=patch&count=3");
  req._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "patch");
  EXPECT_EQ(result.value().count, 3);
}

// DELETE: application/json (same as POST)
TEST(ExtractProtoFromRequestTest, DeleteApplicationJson) {
  auto req = MakeRequest("DELETE", {}, {},
                         R"({"name":"del","count":4})");
  req._headers["Content-Type"] =
      kContentTypeApplicationJson;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "del");
  EXPECT_EQ(result.value().count, 4);
}

// Unknown method: HEAD
TEST(ExtractProtoFromRequestTest, UnknownMethodHead) {
  auto req = MakeRequest("HEAD", {}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsFalse());
  EXPECT_THAT(
      result.message(),
      ::testing::HasSubstr("Unknown HTTP method 'HEAD'"));
}

// Unknown method: OPTIONS
TEST(ExtractProtoFromRequestTest, UnknownMethodOptions) {
  auto req = MakeRequest("OPTIONS", {}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsFalse());
  EXPECT_THAT(result.message(),
              ::testing::HasSubstr(
                  "Unknown HTTP method 'OPTIONS'"));
}

// Unknown method: INVALID
TEST(ExtractProtoFromRequestTest, UnknownMethodInvalid) {
  auto req = MakeRequest("INVALID", {}, {}, "");
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsFalse());
  EXPECT_THAT(result.message(),
              ::testing::HasSubstr(
                  "Unknown HTTP method 'INVALID'"));
}

// Content-Type edge: application/json; charset=utf-8 ->
// should now parse correctly with MatchesContentType
TEST(ExtractProtoFromRequestTest, ContentTypeWithCharset) {
  auto req = MakeRequest(
      "POST", {}, {}, R"({"name":"ignored","count":99})");
  req._headers["Content-Type"] =
      "application/json; charset=utf-8";
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "ignored");
  EXPECT_EQ(result.value().count, 99);
}

// Parse failure: valid extraction but invalid JSON body
TEST(ExtractProtoFromRequestTest, ParseFailureInvalidJson) {
  auto req = MakeRequest("POST", {}, {}, "{invalid");
  req._headers["Content-Type"] =
      kContentTypeApplicationJson;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsFalse());
}

// Form body with duplicate key: last wins (map behavior)
TEST(ExtractProtoFromRequestTest,
     FormDuplicateKeyLastWins) {
  auto req = MakeRequest("POST", {}, {}, "count=1&count=2");
  req._headers["Content-Type"] =
      kContentTypeXWwwFormUrlEncoded;
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().count, 2);
}

// POST: form-urlencoded with charset parameter
TEST(ExtractProtoFromRequestTest,
     PostFormUrlEncodedWithCharset) {
  auto req =
      MakeRequest("POST", {}, {}, "name=foo&count=42");
  req._headers["Content-Type"] =
      "application/x-www-form-urlencoded; charset=utf-8";
  auto result = ExtractProtoFromRequest<Proto>(req);
  ASSERT_THAT(result.ok(), IsTrue());
  EXPECT_EQ(result.value().name, "foo");
  EXPECT_EQ(result.value().count, 42);
}

}  // namespace
}  // namespace cs::net::rpc
