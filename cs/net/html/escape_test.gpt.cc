// cs/net/html/escape_test.gpt.cc
#include <string>

#include "cs/net/html/escape.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cs::net::html {
namespace {

using ::testing::Eq;

TEST(EscapeForHtmlTest, EmptyReturnsEmpty) {
  EXPECT_THAT(EscapeForHtml(""), Eq(""));
}

TEST(EscapeForHtmlTest, NoSpecialCharsPassthrough) {
  EXPECT_THAT(EscapeForHtml("hello world"),
              Eq("hello world"));
}

TEST(EscapeForHtmlTest, AmpersandEscaped) {
  EXPECT_THAT(EscapeForHtml("a&b"), Eq("a&amp;b"));
}

TEST(EscapeForHtmlTest, LessThanEscaped) {
  EXPECT_THAT(EscapeForHtml("<script>"),
              Eq("&lt;script&gt;"));
}

TEST(EscapeForHtmlTest, GreaterThanEscaped) {
  EXPECT_THAT(EscapeForHtml(">"), Eq("&gt;"));
}

TEST(EscapeForHtmlTest, DoubleQuoteEscaped) {
  EXPECT_THAT(EscapeForHtml(R"(say "hi")"),
              Eq("say &quot;hi&quot;"));
}

TEST(EscapeForHtmlTest, SingleQuoteEscaped) {
  EXPECT_THAT(EscapeForHtml("it's"), Eq("it&#39;s"));
}

TEST(EscapeForHtmlTest, MultipleEscapes) {
  EXPECT_THAT(
      EscapeForHtml("<a href=\"x\">link</a>"),
      Eq("&lt;a href=&quot;x&quot;&gt;link&lt;/a&gt;"));
}

TEST(EscapeForHtmlTest, AllFiveEscaped) {
  EXPECT_THAT(EscapeForHtml("&<>\"'"),
              Eq("&amp;&lt;&gt;&quot;&#39;"));
}

}  // namespace
}  // namespace cs::net::html
