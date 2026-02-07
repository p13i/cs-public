// cs/net/html/minify_test.gpt.cc
#include "cs/net/html/minify.hh"

#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cs::net::html::minify {
namespace {

using ::testing::Eq;

std::string Minify(const std::string& html) {
  return MinifyHTML(html);
}

// ---------------------------------------------------------------------------
// Empty and trivial input
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, EmptyReturnsEmpty) {
  EXPECT_THAT(Minify(""), Eq(""));
}

TEST(MinifyHTMLTest, SingleSpaceStripped) {
  EXPECT_THAT(Minify(" "), Eq(""));
}

TEST(MinifyHTMLTest, MultipleSpacesCollapsedToSingle) {
  EXPECT_THAT(Minify("a   b"), Eq("a b"));
}

TEST(MinifyHTMLTest, NewlinesCollapsedToSpace) {
  EXPECT_THAT(Minify("a\nb"), Eq("a b"));
}

TEST(MinifyHTMLTest, TabsCollapsedToSpace) {
  EXPECT_THAT(Minify("a\tb"), Eq("a b"));
}

TEST(MinifyHTMLTest, MixedWhitespaceCollapsed) {
  EXPECT_THAT(Minify("a  \n\t  b"), Eq("a b"));
}

TEST(MinifyHTMLTest, PlainTextNoTagsPassthrough) {
  EXPECT_THAT(Minify("hello world"), Eq("hello world"));
}

TEST(MinifyHTMLTest, PlainTextTrailingWhitespaceTrimmed) {
  EXPECT_THAT(Minify("hello  "), Eq("hello"));
}

TEST(MinifyHTMLTest,
     LeadingWhitespacePreservedAsSingleSpace) {
  EXPECT_THAT(Minify("  hello"), Eq(" hello"));
}

// ---------------------------------------------------------------------------
// Simple tags
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, SimpleOpeningTag) {
  EXPECT_THAT(Minify("<div>"), Eq("<div>"));
}

TEST(MinifyHTMLTest, SimpleClosingTag) {
  EXPECT_THAT(Minify("</div>"), Eq("</div>"));
}

TEST(MinifyHTMLTest, PairedTagsWithContent) {
  EXPECT_THAT(Minify("<div>hello</div>"),
              Eq("<div>hello</div>"));
}

TEST(MinifyHTMLTest, PairedTagsWhitespaceCollapsed) {
  EXPECT_THAT(Minify("<div>  hello   world  </div>"),
              Eq("<div>hello world</div>"));
}

TEST(MinifyHTMLTest, SelfClosingTag) {
  EXPECT_THAT(Minify("<br/>"), Eq("<br/>"));
}

TEST(MinifyHTMLTest, SelfClosingTagWithSpace) {
  EXPECT_THAT(Minify("<br />"), Eq("<br/>"));
}

TEST(MinifyHTMLTest, ImgSelfClosing) {
  EXPECT_THAT(Minify("<img src=\"x.jpg\"/>"),
              Eq("<img src=\"x.jpg\"/>"));
}

TEST(MinifyHTMLTest, NestedTags) {
  EXPECT_THAT(Minify("<div><span>nested</span></div>"),
              Eq("<div><span>nested</span></div>"));
}

TEST(MinifyHTMLTest, NestedTagsWithWhitespace) {
  EXPECT_THAT(
      Minify("<div>  <span>  nested  </span>  </div>"),
      Eq("<div><span>nested</span></div>"));
}

// ---------------------------------------------------------------------------
// Tag names: hyphens, colons
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, TagNameWithHyphen) {
  EXPECT_THAT(Minify("<custom-element>x</custom-element>"),
              Eq("<custom-element>x</custom-element>"));
}

TEST(MinifyHTMLTest, TagNameWithColon) {
  EXPECT_THAT(Minify("<svg:rect>x</svg:rect>"),
              Eq("<svg:rect>x</svg:rect>"));
}

// ---------------------------------------------------------------------------
// Attributes
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, TagWithDoubleQuotedAttribute) {
  EXPECT_THAT(Minify("<a href=\"http://x.com\">link</a>"),
              Eq("<a href=\"http://x.com\">link</a>"));
}

TEST(MinifyHTMLTest, TagWithSingleQuotedAttribute) {
  EXPECT_THAT(Minify("<a href='http://x.com'>link</a>"),
              Eq("<a href='http://x.com'>link</a>"));
}

TEST(MinifyHTMLTest, TagWithMultipleAttributes) {
  EXPECT_THAT(
      Minify("<a href=\"x\" class=\"btn\">link</a>"),
      Eq("<a href=\"x\" class=\"btn\">link</a>"));
}

TEST(MinifyHTMLTest, AttributeWhitespaceNormalized) {
  EXPECT_THAT(Minify("<a  href  =  \"x\"  >link</a>"),
              Eq("<a href=\"x\">link</a>"));
}

TEST(MinifyHTMLTest, AttributeValueWithSpaces) {
  EXPECT_THAT(Minify("<div class=\"foo bar\">x</div>"),
              Eq("<div class=\"foo bar\">x</div>"));
}

TEST(MinifyHTMLTest, BooleanAttribute) {
  EXPECT_THAT(Minify("<button disabled>click</button>"),
              Eq("<button disabled>click</button>"));
}

// ---------------------------------------------------------------------------
// Comments
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, HtmlCommentStripped) {
  EXPECT_THAT(Minify("a<!-- comment -->b"), Eq("ab"));
}

TEST(MinifyHTMLTest, CommentOnly) {
  EXPECT_THAT(Minify("<!-- comment -->"), Eq(""));
}

TEST(MinifyHTMLTest, CommentWithWhitespace) {
  EXPECT_THAT(Minify("a  <!-- x -->  b"), Eq("ab"));
}

TEST(MinifyHTMLTest, ConditionalCommentPreserved) {
  EXPECT_THAT(Minify("<!--[if IE]>ie only<![endif]-->"),
              Eq("<!--[if IE]>ie only<![endif]-->"));
}

TEST(MinifyHTMLTest, ConditionalCommentWithWhitespace) {
  std::string in = "<!--[if IE]>  content  <![endif]-->";
  EXPECT_THAT(Minify(in),
              Eq("<!--[if IE]>  content  <![endif]-->"));
}

// ---------------------------------------------------------------------------
// Processing instructions
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, PhpInstructionPreserved) {
  EXPECT_THAT(Minify("<?php echo 'hi'; ?>"),
              Eq("<?php echo 'hi'; ?>"));
}

TEST(MinifyHTMLTest, XmlDeclarationPreserved) {
  EXPECT_THAT(Minify("<?xml version=\"1.0\"?>"),
              Eq("<?xml version=\"1.0\"?>"));
}

// ---------------------------------------------------------------------------
// Raw tags: script, style, pre, textarea
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, ScriptContentPreserved) {
  EXPECT_THAT(Minify("<script>  var x = 1;  </script>"),
              Eq("<script>  var x = 1;  </script>"));
}

TEST(MinifyHTMLTest, ScriptContentWithNewlines) {
  EXPECT_THAT(Minify("<script>\n  alert('hi');\n</script>"),
              Eq("<script>\n  alert('hi');\n</script>"));
}

TEST(MinifyHTMLTest, StyleContentMinified) {
  EXPECT_THAT(
      Minify("<style>  .foo  {  color: red;  }  </style>"),
      Eq("<style>.foo{color:red}</style>"));
}

TEST(MinifyHTMLTest, StyleCssCommentsStripped) {
  EXPECT_THAT(
      Minify("<style>/* comment */ .x{color:red}</style>"),
      Eq("<style>.x{color:red}</style>"));
}

TEST(MinifyHTMLTest, StyleCalcPreservesSpacing) {
  EXPECT_THAT(
      Minify("<style>.x{width:calc(100% - 20px)}</style>"),
      Eq("<style>.x{width:calc(100%- 20px)}</style>"));
}

TEST(MinifyHTMLTest, PreContentPreserved) {
  EXPECT_THAT(Minify("<pre>  spaces   preserved  </pre>"),
              Eq("<pre>  spaces   preserved  </pre>"));
}

TEST(MinifyHTMLTest, TextareaContentPreserved) {
  EXPECT_THAT(
      Minify("<textarea>  default  value  </textarea>"),
      Eq("<textarea>  default  value  </textarea>"));
}

TEST(MinifyHTMLTest, EmptyStyleTag) {
  EXPECT_THAT(Minify("<style></style>"),
              Eq("<style></style>"));
}

TEST(MinifyHTMLTest, EmptyScriptTag) {
  EXPECT_THAT(Minify("<script></script>"),
              Eq("<script></script>"));
}

// ---------------------------------------------------------------------------
// CSS minification (via style tag)
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, CssMultipleRules) {
  EXPECT_THAT(
      Minify("<style>.a{color:red}.b{margin:0}</style>"),
      Eq("<style>.a{color:red}.b{margin:0}</style>"));
}

TEST(MinifyHTMLTest, CssStringInValue) {
  EXPECT_THAT(
      Minify("<style>.x{font-family:\"Arial\"}</style>"),
      Eq("<style>.x{font-family:\"Arial\"}</style>"));
}

TEST(MinifyHTMLTest, CssEscapedQuoteInString) {
  EXPECT_THAT(Minify("<style>.x{content:\"\\\"\"}</style>"),
              Eq("<style>.x{content:\"\\\"\"}</style>"));
}

TEST(MinifyHTMLTest, CssSemicolonBeforeClosingBrace) {
  EXPECT_THAT(Minify("<style>.x{color:red;}</style>"),
              Eq("<style>.x{color:red}</style>"));
}

TEST(MinifyHTMLTest, CssMediaQuery) {
  EXPECT_THAT(
      Minify("<style>@media "
             "(max-width:600px){.x{display:none}}</style>"),
      Eq("<style>@media(max-width:600px){.x{display:none}}<"
         "/style>"));
}

TEST(MinifyHTMLTest, CssSelectorWithCombinators) {
  EXPECT_THAT(Minify("<style>div > p{margin:0}</style>"),
              Eq("<style>div>p{margin:0}</style>"));
}

TEST(MinifyHTMLTest, CssNestedCalc) {
  EXPECT_THAT(Minify("<style>.x{width:calc(100% - "
                     "calc(10px + 5px))}</style>"),
              Eq("<style>.x{width:calc(100%- calc(10px + "
                 "5px))}</style>"));
}

// ---------------------------------------------------------------------------
// Stream overloads
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, IstreamOstreamOverload) {
  std::istringstream in("<div>  hello  </div>");
  std::ostringstream out;
  MinifyHTML(in, out);
  EXPECT_THAT(out.str(), Eq("<div>hello</div>"));
}

TEST(MinifyHTMLTest, StringstreamOverload) {
  std::stringstream ss("<p>  world  </p>");
  std::stringstream result = MinifyHTML(std::move(ss));
  EXPECT_THAT(result.str(), Eq("<p>world</p>"));
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

TEST(MinifyHTMLTest, AdjacentTagsNoSpace) {
  EXPECT_THAT(Minify("<span>a</span><span>b</span>"),
              Eq("<span>a</span><span>b</span>"));
}

TEST(MinifyHTMLTest, InlineElementWhitespace) {
  EXPECT_THAT(Minify("<p>a <b>bold</b> c</p>"),
              Eq("<p>a<b>bold</b>c</p>"));
}

TEST(MinifyHTMLTest, OnlyTags) {
  EXPECT_THAT(Minify("<div></div>"), Eq("<div></div>"));
}

TEST(MinifyHTMLTest, DocumentStructure) {
  std::string html = R"(<!DOCTYPE html>
<html>
  <head><title>Test</title></head>
  <body><p>Hello</p></body>
</html>)";
  EXPECT_THAT(Minify(html),
              Eq("<!DOCTYPE "
                 "html><html><head><title>Test</title></"
                 "head><body><p>Hello</p></body></html>"));
}

TEST(MinifyHTMLTest, ClosingTagWithWhitespaceBeforeName) {
  EXPECT_THAT(Minify("<div></ div>"), Eq("<div></div>"));
}

TEST(MinifyHTMLTest, DoctypePreserved) {
  EXPECT_THAT(Minify("<!DOCTYPE html>"),
              Eq("<!DOCTYPE html>"));
}

TEST(MinifyHTMLTest, CaseInsensitiveRawTagScript) {
  EXPECT_THAT(Minify("<SCRIPT>  x  </SCRIPT>"),
              Eq("<SCRIPT>  x  </SCRIPT>"));
}

TEST(MinifyHTMLTest, StyleTagCaseInsensitiveClose) {
  EXPECT_THAT(Minify("<style>.x{color:red}</STYLE>"),
              Eq("<style>.x{color:red}</STYLE>"));
}

TEST(MinifyHTMLTest, MultipleStyleTags) {
  EXPECT_THAT(
      Minify(
          "<style>.a{x:1}</style><style>.b{y:2}</style>"),
      Eq("<style>.a{x:1}</style><style>.b{y:2}</style>"));
}

TEST(MinifyHTMLTest, NestedRawTagInScript) {
  std::string in = "<script>var s='<div>';</script>";
  EXPECT_THAT(Minify(in),
              Eq("<script>var s='<div>';</script>"));
}

TEST(MinifyHTMLTest, UnclosedCommentReturnsPartial) {
  std::string in = "a<!-- no end";
  std::string result = Minify(in);
  EXPECT_THAT(result, Eq("a"));
}

TEST(MinifyHTMLTest, CssUnitsPreserved) {
  EXPECT_THAT(
      Minify("<style>.x{width:100px;height:50%}</style>"),
      Eq("<style>.x{width:100px;height:50%}</style>"));
}

TEST(MinifyHTMLTest, CssImportant) {
  EXPECT_THAT(
      Minify("<style>.x{color:red!important}</style>"),
      Eq("<style>.x{color:red!important}</style>"));
}

TEST(MinifyHTMLTest, CssAttributeSelector) {
  EXPECT_THAT(
      Minify("<style>[type=\"text\"]{border:1px}</style>"),
      Eq("<style>[type=\"text\"]{border:1px}</style>"));
}

}  // namespace
}  // namespace cs::net::html::minify
