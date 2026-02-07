// cs/net/http/response_test.gpt.cc
#include "cs/net/http/response.hh"

#include <sstream>
#include <string>

#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Error;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Response;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::IsFalse;
using ::testing::IsTrue;
}  // namespace

// -----------------------------------------------------------------------------
// Response default constructor
// -----------------------------------------------------------------------------

TEST(ResponseTest, DefaultConstructor) {
  Response r;
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r.status().name, Eq("OK"));
  EXPECT_THAT(r.body(), Eq("200 OK"));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
}

// -----------------------------------------------------------------------------
// Response(content_type, s)
// -----------------------------------------------------------------------------

TEST(ResponseTest, ConstructorContentTypeAndString) {
  Response r("application/json", "{\"x\":1}");
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r._content_type, Eq("application/json"));
  EXPECT_THAT(r.body(), Eq("{\"x\":1}"));
}

// -----------------------------------------------------------------------------
// Response(Status)
// -----------------------------------------------------------------------------

TEST(ResponseTest, ConstructorStatusOnly) {
  Response r(HTTP_400_BAD_REQUEST);
  EXPECT_THAT(r.status().code, Eq(400u));
  EXPECT_THAT(r.status().name, Eq("BAD REQUEST"));
  EXPECT_THAT(r.body(), Eq("400 BAD REQUEST"));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
}

// -----------------------------------------------------------------------------
// Response(Status, content_type, body)
// -----------------------------------------------------------------------------

TEST(ResponseTest, ConstructorStatusContentTypeBody) {
  Response r(HTTP_500_INTERNAL_SERVER_ERROR, "text/plain",
             "Internal error");
  EXPECT_THAT(r.status().code, Eq(500u));
  EXPECT_THAT(r._content_type, Eq("text/plain"));
  EXPECT_THAT(r.body(), Eq("Internal error"));
}

// -----------------------------------------------------------------------------
// Response(Status, content_type, stringstream)
// -----------------------------------------------------------------------------

TEST(ResponseTest,
     ConstructorStatusContentTypeStringstream) {
  std::stringstream ss;
  ss << "<p>hi</p>";
  Response r(HTTP_200_OK, kContentTypeTextHtml, ss.str());
  EXPECT_THAT(r.body(), Eq("<p>hi</p>"));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextHtml));
}

// -----------------------------------------------------------------------------
// Response(Result)
// -----------------------------------------------------------------------------

TEST(ResponseTest, ConstructorFromOkResult) {
  Response r{Ok()};
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
  EXPECT_THAT(r.body(), Eq("200 OK"));
}

TEST(ResponseTest, ConstructorFromErrorResult) {
  Response r(Error("something failed"));
  EXPECT_THAT(r.status().code, Eq(400u));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
  EXPECT_THAT(r.body(), HasSubstr("400"));
  EXPECT_THAT(r.body(), HasSubstr("something failed"));
}

TEST(ResponseTest, ConstructorFromErrorResultEmptyMessage) {
  Response r(Error(""));
  EXPECT_THAT(r.status().code, Eq(400u));
  EXPECT_THAT(r.body(), Eq("400 BAD REQUEST"));
}

TEST(ResponseTest, ConstructorFromNotFoundResult) {
  Response r(NotFoundError("No values found"));
  EXPECT_THAT(r.status().code, Eq(404u));
  EXPECT_THAT(r.status().name, Eq("NOT FOUND"));
  EXPECT_THAT(r.body(), HasSubstr("404"));
  EXPECT_THAT(r.body(), HasSubstr("Not found"));
}

// -----------------------------------------------------------------------------
// Response(status_str)
// -----------------------------------------------------------------------------

TEST(ResponseTest, ConstructorFromStatusStr200) {
  Response r("200 OK");
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
}

TEST(ResponseTest, ConstructorFromStatusStr400) {
  Response r("400 BAD REQUEST");
  EXPECT_THAT(r.status().code, Eq(400u));
}

TEST(ResponseTest,
     ConstructorFromStatusStrUnknownLeavesDefault) {
  Response r("999 UNKNOWN");
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextPlain));
}

// -----------------------------------------------------------------------------
// Parse
// -----------------------------------------------------------------------------

TEST(ResponseTest, ParseMinimal200) {
  std::string raw =
      "HTTP/1.1 200 OK\r\n"
      "X: y\r\n"
      "\r\n";
  Response r;
  auto result = r.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r.body(), Eq(""));
}

TEST(ResponseTest, ParseWithLeadingWhitespace) {
  std::string raw =
      "  \r\n  HTTP/1.1 200 OK\r\n"
      "X: y\r\n"
      "\r\n";
  Response r;
  auto result = r.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(r.status().code, Eq(200u));
}

TEST(ResponseTest, ParseWithHeaders) {
  std::string raw =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "X-Custom: value\r\n"
      "\r\n"
      "body";
  Response r;
  auto result = r.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(r._headers.size(), Eq(2u));
  EXPECT_THAT(r._headers.at("Content-Type"),
              Eq("text/html"));
  EXPECT_THAT(r._headers.at("X-Custom"), Eq("value"));
  EXPECT_THAT(r.body(), Eq("body"));
}

TEST(ResponseTest, Parse400) {
  std::string raw =
      "HTTP/1.1 400 BAD REQUEST\r\n"
      "X: y\r\n"
      "\r\n";
  Response r;
  auto result = r.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(r.status().code, Eq(400u));
}

TEST(ResponseTest, Parse500) {
  std::string raw =
      "HTTP/1.1 500 INTERNAL SERVER ERROR\r\n"
      "X: y\r\n"
      "\r\n";
  Response r;
  auto result = r.Parse(raw);
  ASSERT_OK(result);
  EXPECT_THAT(r.status().code, Eq(500u));
}

TEST(ResponseTest, ParseEmptyStringFails) {
  Response r;
  auto result = r.Parse("");
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(), HasSubstr("empty"));
}

TEST(ResponseTest, ParseWhitespaceOnlyFails) {
  Response r;
  auto result = r.Parse("   \r\n   ");
  EXPECT_NOK(result);
}

TEST(ResponseTest, ParseNonHttp11Fails) {
  Response r;
  auto result = r.Parse("HTTP/1.0 200 OK\r\n\r\n");
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(), HasSubstr("HTTP/1.1"));
}

TEST(ResponseTest, ParseExpectsSpaceAfterColonInHeader) {
  Response r;
  auto result = r.Parse(
      "HTTP/1.1 200 OK\r\n"
      "Host:localhost\r\n"
      "\r\n");
  EXPECT_NOK(result);
}

// -----------------------------------------------------------------------------
// SetHeader
// -----------------------------------------------------------------------------

TEST(ResponseTest, SetHeaderAddsHeaderAndReturnsSelf) {
  Response r;
  r.SetHeader("X-Test", "value");
  EXPECT_THAT(r._headers["X-Test"], Eq("value"));
}

TEST(ResponseTest, SetHeaderChainable) {
  Response r;
  r.SetHeader("A", "1");
  r.SetHeader("B", "2");
  EXPECT_THAT(r._headers["A"], Eq("1"));
  EXPECT_THAT(r._headers["B"], Eq("2"));
}

// -----------------------------------------------------------------------------
// operator<< and to_string
// -----------------------------------------------------------------------------

TEST(ResponseTest,
     ToStringContainsStatusAndContentTypeAndBody) {
  Response r(HTTP_200_OK, kContentTypeTextPlain, "hello");
  std::string s = r.to_string();
  EXPECT_THAT(s, HasSubstr("HTTP/1.1"));
  EXPECT_THAT(s, HasSubstr("200 OK"));
  EXPECT_THAT(s, HasSubstr("Content-Type: text/plain"));
  EXPECT_THAT(s, HasSubstr("Content-Length: 5"));
  EXPECT_THAT(s, HasSubstr("hello"));
}

TEST(ResponseTest, ToStringIncludesCustomHeaders) {
  Response r(HTTP_200_OK, kContentTypeTextPlain, "");
  r.SetHeader("X-Custom", "v");
  std::string s = r.to_string();
  EXPECT_THAT(s, HasSubstr("X-Custom: v"));
}

// -----------------------------------------------------------------------------
// HtmlResponse (response.cc)
// -----------------------------------------------------------------------------

TEST(ResponseTest, HtmlResponseString) {
  Response r = HtmlResponse("<html></html>");
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextHtml));
  EXPECT_THAT(r.body(), Eq("<html></html>"));
}

TEST(ResponseTest, HtmlResponseStringstream) {
  std::stringstream ss;
  ss << "<body>hi</body>";
  Response r = HtmlResponse(std::move(ss));
  EXPECT_THAT(r.status().code, Eq(200u));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextHtml));
  EXPECT_THAT(r.body(), Eq("<body>hi</body>"));
}

TEST(ResponseTest, HtmlResponseEmptyBody) {
  Response r = HtmlResponse("");
  EXPECT_THAT(r.body(), Eq(""));
  EXPECT_THAT(r._content_type, Eq(kContentTypeTextHtml));
}
