// cs/net/http/request_test.gpt.cc
#include "cs/net/http/request.hh"

#include <map>
#include <sstream>
#include <string>

#include "cs/testing/extensions.gpt.hh"
#include "cs/util/string.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Result;
using ::cs::net::http::Request;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::UnorderedElementsAre;
}  // namespace

class RequestTest : public ::testing::Test {
 protected:
  Request request_;
};

// -----------------------------------------------------------------------------
// Default constructor and accessors
// -----------------------------------------------------------------------------

TEST_F(RequestTest, DefaultConstructor) {
  EXPECT_THAT(request_.method(), Eq(""));
  EXPECT_THAT(request_.path(), Eq("/"));
  EXPECT_THAT(request_.body(), Eq(""));
  EXPECT_THAT(request_.headers().empty(), IsTrue());
  EXPECT_THAT(request_.query_params().empty(), IsTrue());
}

// -----------------------------------------------------------------------------
// Parse - minimal GET
// -----------------------------------------------------------------------------

TEST_F(RequestTest, ParseMinimalGet) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.method(), Eq("GET"));
  EXPECT_THAT(request_.path(), Eq("/"));
  EXPECT_THAT(request_.body(), Eq(""));
  EXPECT_THAT(request_.headers().size(), Eq(1u));
  EXPECT_THAT(request_.headers().at("Host"), Eq("x"));
}

TEST_F(RequestTest, ParseGetWithPath) {
  std::string raw =
      "GET /foo/bar HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.method(), Eq("GET"));
  EXPECT_THAT(request_.path(), Eq("/foo/bar"));
}

TEST_F(RequestTest, ParseGetWithQueryString) {
  std::string raw =
      "GET /search?q=hello&page=1 HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.path(), Eq("/search"));
  EXPECT_THAT(request_.query_params().size(), Eq(2u));
  EXPECT_THAT(request_.GetQueryParam("q").value(),
              Eq("hello"));
  EXPECT_THAT(request_.GetQueryParam("page").value(),
              Eq("1"));
}

TEST_F(RequestTest, GetQueryParamMissing) {
  std::string raw =
      "GET /search?q=hello HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  auto result = request_.GetQueryParam("missing");
  EXPECT_NOK(result);
}

// -----------------------------------------------------------------------------
// Parse - headers
// -----------------------------------------------------------------------------

TEST_F(RequestTest, ParseWithHeaders) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host: localhost:8080\r\n"
      "Accept: text/html\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.headers().size(), Eq(2u));
  EXPECT_THAT(request_.headers().at("Host"),
              Eq("localhost:8080"));
  EXPECT_THAT(request_.headers().at("Accept"),
              Eq("text/html"));
}

TEST_F(RequestTest, ParseWithSingleHeader) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "X-Custom: value\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.headers().at("X-Custom"),
              Eq("value"));
}

// -----------------------------------------------------------------------------
// Parse - body
// -----------------------------------------------------------------------------

TEST_F(RequestTest, ParseWithBody) {
  std::string raw =
      "POST / HTTP/1.1\r\n"
      "Content-Length: 11\r\n"
      "\r\n"
      "hello world";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.method(), Eq("POST"));
  EXPECT_THAT(request_.body(), Eq("hello world"));
}

TEST_F(RequestTest, ParseEmptyBody) {
  std::string raw =
      "POST / HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(request_.body(), Eq(""));
}

// -----------------------------------------------------------------------------
// Parse - errors
// -----------------------------------------------------------------------------

TEST_F(RequestTest, ParseRejectsNonHttp11) {
  std::string raw =
      "GET / HTTP/1.0\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  EXPECT_NOK(result);
  EXPECT_TRUE(
      CS_STRING_CONTAINS(result.message(), "HTTP/1.1"));
}

TEST_F(RequestTest, ParseExpectsSpaceAfterColonInHeader) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host:localhost\r\n"
      "\r\n";
  auto result = request_.Parse(raw);
  EXPECT_NOK(result);
  EXPECT_TRUE(
      CS_STRING_CONTAINS(result.message(), "space"));
}

// -----------------------------------------------------------------------------
// NormalizedHost
// -----------------------------------------------------------------------------

TEST_F(RequestTest, NormalizedHostReturnsLowercase) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host: LOCALHOST\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  auto host = request_.NormalizedHost();
  ASSERT_OK(host);
  EXPECT_THAT(host.value(), Eq("localhost"));
}

TEST_F(RequestTest, NormalizedHostStripsPort) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host: example.com:8080\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  auto host = request_.NormalizedHost();
  ASSERT_OK(host);
  EXPECT_THAT(host.value(), Eq("example.com"));
}

TEST_F(RequestTest, NormalizedHostMissingHeader) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "X-Other: y\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  auto host = request_.NormalizedHost();
  EXPECT_NOK(host);
}

TEST_F(RequestTest,
       NormalizedHostCaseInsensitiveHeaderName) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "HOST: foo.com\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  auto host = request_.NormalizedHost();
  ASSERT_OK(host);
  EXPECT_THAT(host.value(), Eq("foo.com"));
}

// -----------------------------------------------------------------------------
// ParseFormUrlEncoded and UrlDecode (via form)
// -----------------------------------------------------------------------------

TEST_F(RequestTest, ParseFormUrlEncodedSimple) {
  std::string body = "name=Alice&age=30";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form.size(), Eq(2u));
  EXPECT_THAT(form["name"], Eq("Alice"));
  EXPECT_THAT(form["age"], Eq("30"));
}

TEST_F(RequestTest, ParseFormUrlEncodedPlusToSpace) {
  std::string body = "msg=hello+world";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form["msg"], Eq("hello world"));
}

TEST_F(RequestTest, ParseFormUrlEncodedPercentDecode) {
  std::string body = "x=%41%42%43";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form["x"], Eq("ABC"));
}

TEST_F(RequestTest, ParseFormUrlEncodedEmptyValue) {
  std::string body = "key=";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form["key"], Eq(""));
}

TEST_F(RequestTest, ParseFormUrlEncodedEmptyBody) {
  auto form = request_.ParseFormUrlEncoded("");
  EXPECT_THAT(form.empty(), IsTrue());
}

TEST_F(RequestTest, ParseFormUrlEncodedSinglePair) {
  std::string body = "a=b";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form.size(), Eq(1u));
  EXPECT_THAT(form["a"], Eq("b"));
}

TEST_F(RequestTest, ParseFormUrlEncodedNoEqualsOmitted) {
  std::string body = "lonely";
  auto form = request_.ParseFormUrlEncoded(body);
  EXPECT_THAT(form.count("lonely"), Eq(0u));
}

// -----------------------------------------------------------------------------
// summary
// -----------------------------------------------------------------------------

TEST_F(RequestTest, Summary) {
  std::string raw =
      "GET /search?q=hi HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  std::string s = request_.summary();
  EXPECT_TRUE(CS_STRING_CONTAINS(s, "GET"));
  EXPECT_TRUE(CS_STRING_CONTAINS(s, "/search"));
  EXPECT_TRUE(CS_STRING_CONTAINS(s, "q"));
}

// -----------------------------------------------------------------------------
// operator>> (buffer to request)
// -----------------------------------------------------------------------------

TEST_F(RequestTest, OperatorStreamParse) {
  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "GET / HTTP/1.1\r\n"
           "Host: x\r\n"
           "\r\n");
  Request req;
  auto result = static_cast<Result>(buffer >> req);
  ASSERT_OK(result);
  EXPECT_THAT(req.method(), Eq("GET"));
  EXPECT_THAT(req.path(), Eq("/"));
}

// -----------------------------------------------------------------------------
// set_body
// -----------------------------------------------------------------------------

TEST_F(RequestTest, SetBodySetsBodyAndAppendsToStr) {
  std::string raw =
      "POST / HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  request_.set_body("data");
  EXPECT_THAT(request_.body(), Eq("data"));
  std::ostringstream os;
  os << request_;
  EXPECT_THAT(os.str(), Eq(raw + "data"));
}

// -----------------------------------------------------------------------------
// operator<<
// -----------------------------------------------------------------------------

TEST_F(RequestTest, OperatorOstreamPrintsRawStr) {
  std::string raw =
      "GET / HTTP/1.1\r\n"
      "Host: x\r\n"
      "\r\n";
  EXPECT_OK(request_.Parse(raw));
  std::ostringstream os;
  os << request_;
  EXPECT_THAT(os.str(), Eq(raw));
}
