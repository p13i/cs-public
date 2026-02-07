// cs/net/http/parsers_test.gpt.cc
#include "cs/net/http/parsers.hh"

#include <map>
#include <string>

#include "cs/testing/extensions.gpt.hh"
#include "cs/util/string.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::net::http::parsers::AtEndOfLine;
using ::cs::net::http::parsers::IncrementCursor;
using ::cs::net::http::parsers::ParsePath;
using ::cs::net::http::parsers::ReadThroughNewline;
using ::cs::net::http::parsers::ReadWord;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::UnorderedElementsAre;
}  // namespace

// -----------------------------------------------------------------------------
// AtEndOfLine
// -----------------------------------------------------------------------------

TEST(AtEndOfLineTest, EmptyStringCursorZero) {
  EXPECT_NOK(AtEndOfLine("", 0));
}

TEST(AtEndOfLineTest, CursorPastEnd) {
  EXPECT_NOK(AtEndOfLine("ab", 3));
}

TEST(AtEndOfLineTest, CursorAtNewline) {
  ASSERT_OK(AtEndOfLine("a\nb", 1));
}

TEST(AtEndOfLineTest, CursorAtCrLf) {
  ASSERT_OK(AtEndOfLine("a\r\nb", 1));
}

TEST(AtEndOfLineTest, CursorAtCrLfAtOffsetZero) {
  ASSERT_OK(AtEndOfLine("\r\n", 0));
}

TEST(AtEndOfLineTest, CursorAtCrLfAtOffsetOne) {
  ASSERT_OK(AtEndOfLine("\r\n", 1));
}

TEST(AtEndOfLineTest, CursorNotAtLineEnd) {
  EXPECT_NOK(AtEndOfLine("abc", 1));
}

TEST(AtEndOfLineTest, SingleCharNewline) {
  ASSERT_OK(AtEndOfLine("\n", 0));
}

TEST(AtEndOfLineTest, CursorAtEndOfStringNoNewline) {
  EXPECT_NOK(AtEndOfLine("abc", 3));
}

// -----------------------------------------------------------------------------
// IncrementCursor
// -----------------------------------------------------------------------------

TEST(IncrementCursorTest, IncrementFromZero) {
  unsigned int cursor = 0;
  ASSERT_OK(IncrementCursor("ab", &cursor));
  EXPECT_THAT(cursor, Eq(1u));
}

TEST(IncrementCursorTest, IncrementToEndFails) {
  unsigned int cursor = 0;
  EXPECT_NOK(IncrementCursor("a", &cursor));
}

TEST(IncrementCursorTest, CursorAtLastChar) {
  unsigned int cursor = 1;
  EXPECT_NOK(IncrementCursor("ab", &cursor));
}

TEST(IncrementCursorTest, CursorPastEnd) {
  unsigned int cursor = 5;
  EXPECT_NOK(IncrementCursor("ab", &cursor));
}

TEST(IncrementCursorTest, EmptyString) {
  unsigned int cursor = 0;
  EXPECT_NOK(IncrementCursor("", &cursor));
}

TEST(IncrementCursorTest,
     ErrorMessageContainsCursorAndLength) {
  unsigned int cursor = 2;
  auto result = IncrementCursor("ab", &cursor);
  EXPECT_NOK(result);
  std::string msg = result.message();
  EXPECT_TRUE(CS_STRING_CONTAINS(msg, "*cursor="));
  EXPECT_TRUE(CS_STRING_CONTAINS(msg, "str.length()="));
}

// -----------------------------------------------------------------------------
// ReadWord
// -----------------------------------------------------------------------------

TEST(ReadWordTest, ReadUntilSpace) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(
      ReadWord("GET / HTTP/1.1", &cursor, &token, " "));
  EXPECT_THAT(token, Eq("GET"));
  EXPECT_THAT(cursor, Eq(3u));
}

TEST(ReadWordTest, ReadUntilNewline) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(ReadWord("value\nnext", &cursor, &token, "\n"));
  EXPECT_THAT(token, Eq("value"));
  EXPECT_THAT(cursor, Eq(5u));
}

TEST(ReadWordTest, ReadSkipsCarriageReturn) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(ReadWord("val\rue\n", &cursor, &token, "\n"));
  EXPECT_THAT(token, Eq("value"));
}

TEST(ReadWordTest, ReadEmptyWordAtDelimiter) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(ReadWord("?\n", &cursor, &token, "?\n"));
  EXPECT_THAT(token, Eq(""));
  EXPECT_THAT(cursor, Eq(0u));
}

TEST(ReadWordTest,
     ReadWordWithEndingNewlineInSetStopsAtNewline) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(ReadWord("path\n", &cursor, &token, "\n"));
  EXPECT_THAT(token, Eq("path"));
  EXPECT_THAT(cursor, Eq(4u));
}

TEST(ReadWordTest,
     CursorExceedsLengthWhenEndingTokensNoNewline) {
  unsigned int cursor = 0;
  std::string token;
  EXPECT_NOK(ReadWord("abc", &cursor, &token, " "));
}

TEST(ReadWordTest, ReadUntilColon) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(
      ReadWord("Host: localhost", &cursor, &token, ":"));
  EXPECT_THAT(token, Eq("Host"));
  EXPECT_THAT(cursor, Eq(4u));
}

TEST(ReadWordTest, ReadMultipleWordsSequentially) {
  unsigned int cursor = 0;
  std::string t1, t2, t3;
  ASSERT_OK(ReadWord("one two three ", &cursor, &t1, " "));
  ASSERT_OK(IncrementCursor("one two three ", &cursor));
  ASSERT_OK(ReadWord("one two three ", &cursor, &t2, " "));
  ASSERT_OK(IncrementCursor("one two three ", &cursor));
  ASSERT_OK(ReadWord("one two three ", &cursor, &t3, " "));
  EXPECT_THAT(t1, Eq("one"));
  EXPECT_THAT(t2, Eq("two"));
  EXPECT_THAT(t3, Eq("three"));
}

TEST(ReadWordTest, EndingTokensContainMultipleChars) {
  unsigned int cursor = 0;
  std::string token;
  ASSERT_OK(ReadWord("foo&bar=baz", &cursor, &token, "=&"));
  EXPECT_THAT(token, Eq("foo"));
  EXPECT_THAT(cursor, Eq(3u));
}

// -----------------------------------------------------------------------------
// ReadThroughNewline
// -----------------------------------------------------------------------------

TEST(ReadThroughNewlineTest, ReadThroughLf) {
  unsigned int cursor = 0;
  ASSERT_OK(ReadThroughNewline("line\nrest", &cursor));
  EXPECT_THAT(cursor, Eq(5u));
}

TEST(ReadThroughNewlineTest, ReadThroughCrLf) {
  unsigned int cursor = 0;
  ASSERT_OK(ReadThroughNewline("line\r\nrest", &cursor));
  EXPECT_THAT(cursor, Eq(6u));
}

TEST(ReadThroughNewlineTest, NoNewlineInString) {
  unsigned int cursor = 0;
  EXPECT_NOK(ReadThroughNewline("noline", &cursor));
}

TEST(ReadThroughNewlineTest, EmptyString) {
  unsigned int cursor = 0;
  EXPECT_NOK(ReadThroughNewline("", &cursor));
}

TEST(ReadThroughNewlineTest, CursorStartsAtNewline) {
  unsigned int cursor = 4;
  ASSERT_OK(ReadThroughNewline("line\n", &cursor));
  EXPECT_THAT(cursor, Eq(5u));
}

// -----------------------------------------------------------------------------
// ParsePath
// -----------------------------------------------------------------------------

TEST(ParsePathTest, PathOnlyNoQuery) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(
      ParsePath("/foo/bar", &just_path, &query_params));
  EXPECT_THAT(just_path, Eq(""));
  EXPECT_THAT(query_params.empty(), IsTrue());
}

TEST(ParsePathTest, PathWithQueryOneParam) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(ParsePath("/search?q=hello", &just_path,
                      &query_params));
  EXPECT_THAT(just_path, Eq("/search"));
  EXPECT_THAT(query_params.size(), Eq(1u));
  EXPECT_THAT(query_params["q"], Eq("hello"));
}

TEST(ParsePathTest, PathWithQueryMultipleParams) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(ParsePath("/api?a=1&b=2&c=three", &just_path,
                      &query_params));
  EXPECT_THAT(just_path, Eq("/api"));
  EXPECT_THAT(query_params.size(), Eq(3u));
  EXPECT_THAT(query_params["a"], Eq("1"));
  EXPECT_THAT(query_params["b"], Eq("2"));
  EXPECT_THAT(query_params["c"], Eq("three"));
}

TEST(ParsePathTest, RootWithEmptyQuery) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  EXPECT_NOK(ParsePath("/?", &just_path, &query_params));
}

TEST(ParsePathTest, QueryWithEmptyValue) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  EXPECT_NOK(
      ParsePath("/x?key=", &just_path, &query_params));
}

TEST(ParsePathTest, QueryWithEmptyKey) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(
      ParsePath("/x?=value", &just_path, &query_params));
  EXPECT_THAT(query_params[""], Eq("value"));
}

TEST(ParsePathTest, PathWithQuestionMarkOnly) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  EXPECT_NOK(
      ParsePath("/path?", &just_path, &query_params));
}

TEST(ParsePathTest, SingleSlash) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(ParsePath("/", &just_path, &query_params));
  EXPECT_THAT(just_path, Eq(""));
  EXPECT_THAT(query_params.empty(), IsTrue());
}

TEST(ParsePathTest, EmptyPath) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(ParsePath("", &just_path, &query_params));
  EXPECT_THAT(just_path, Eq(""));
}

TEST(ParsePathTest, QueryValueWithAmpersandEncoded) {
  std::string just_path;
  std::map<std::string, std::string> query_params;
  ASSERT_OK(
      ParsePath("/?a=1&b=2", &just_path, &query_params));
  EXPECT_THAT(query_params["a"], Eq("1"));
  EXPECT_THAT(query_params["b"], Eq("2"));
}
