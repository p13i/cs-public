// cs/net/html/dom_test.gpt.cc
#include "cs/net/html/dom.hh"

#include <map>
#include <string>

#include "gtest/gtest.h"

namespace cs::net::html::dom {
namespace {

TEST(DomTest, HtmlBasic) {
  auto result = html("<head></head>", "<body>hi</body>");
  EXPECT_EQ(result,
            "<!DOCTYPE html><html><head></head>"
            "<body>hi</body></html>");
}

TEST(DomTest, DivMultiChild) {
  auto result = div(std::string("a"), std::string("b"));
  EXPECT_EQ(result, "<div>ab</div>");
}

TEST(DomTest, H1) {
  EXPECT_EQ(h1("Title"), "<h1>Title</h1>");
}

TEST(DomTest, H2) { EXPECT_EQ(h2("Sub"), "<h2>Sub</h2>"); }

TEST(DomTest, H3) {
  EXPECT_EQ(h3("Sub2"), "<h3>Sub2</h3>");
}

TEST(DomTest, Code) {
  EXPECT_EQ(code("x"), "<code>x</code>");
}

TEST(DomTest, Strong) {
  EXPECT_EQ(strong("bold"), "<strong>bold</strong>");
}

TEST(DomTest, AnchorBasic) {
  auto result = a("http://example.com", "click");
  EXPECT_EQ(result,
            "<a href=\"http://example.com\">click</a>");
}

TEST(DomTest, Hr) { EXPECT_EQ(hr(), "<hr/>"); }

TEST(DomTest, Nbsp) { EXPECT_EQ(nbsp(), "&nbsp;"); }

TEST(DomTest, Copy) { EXPECT_EQ(copy(), "&copy;"); }

TEST(DomTest, StyleTag) {
  EXPECT_EQ(style("body{}"), "<style>body{}</style>");
}

TEST(DomTest, ScriptString) {
  EXPECT_EQ(script("alert(1)"),
            "<script type=\"text/javascript\">"
            "alert(1)</script>");
}

TEST(DomTest, BuildAttrsEmpty) {
  EXPECT_EQ(BuildAttrs({}), "");
}

TEST(DomTest, BuildAttrsOne) {
  std::map<std::string, std::string> attrs = {
      {"class", "main"}};
  EXPECT_EQ(BuildAttrs(attrs), " class=\"main\"");
}

TEST(DomTest, InputTag) {
  std::map<std::string, std::string> attrs = {
      {"type", "text"}, {"name", "q"}};
  auto result = input(attrs);
  EXPECT_NE(result.find("<input"), std::string::npos);
  EXPECT_NE(result.find("type=\"text\""),
            std::string::npos);
  EXPECT_NE(result.find("/>"), std::string::npos);
}

TEST(DomTest, CheckboxSetsType) {
  std::map<std::string, std::string> attrs = {
      {"name", "agree"}};
  auto result = checkbox(attrs);
  EXPECT_NE(result.find("type=\"checkbox\""),
            std::string::npos);
}

TEST(DomTest, DivWithAttrs) {
  std::map<std::string, std::string> attrs = {
      {"id", "main"}};
  auto result = div(attrs, std::string("content"));
  EXPECT_NE(result.find("id=\"main\""), std::string::npos);
  EXPECT_NE(result.find(">content</div>"),
            std::string::npos);
}

TEST(DomTest, MetaWithMap) {
  std::map<std::string, std::string> attrs = {
      {"charset", "utf-8"}};
  auto result = meta(attrs);
  EXPECT_NE(result.find("charset=\"utf-8\""),
            std::string::npos);
  EXPECT_NE(result.find("/>"), std::string::npos);
}

}  // namespace
}  // namespace cs::net::html::dom
