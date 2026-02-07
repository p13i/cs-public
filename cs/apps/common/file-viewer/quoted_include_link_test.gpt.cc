// cs/apps/common/file-viewer/quoted_include_link_test.gpt.cc
#include <string>

#include "cs/apps/common/file-viewer/quoted_include_link.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::common::file_viewer::
    RenderCodeLineWithQuotedIncludeLinks;
using ::cs::apps::common::file_viewer::
    ResolveQuotedIncludePath;
using ::testing::HasSubstr;
using ::testing::Not;
}  // namespace

namespace {

// ---- ResolveQuotedIncludePath (path resolution only) ----

TEST(ResolveQuotedIncludePathTest, RelativeToCurrentDir) {
  EXPECT_EQ(ResolveQuotedIncludePath("foo/bar.h",
                                     "a/main.cc", ""),
            "a/foo/bar.h");
  EXPECT_EQ(
      ResolveQuotedIncludePath("h.h", "dir/file.cc", ""),
      "dir/h.h");
}

TEST(ResolveQuotedIncludePathTest,
     RepoRelativeWhenPrefixMatches) {
  EXPECT_EQ(ResolveQuotedIncludePath(
                "cs/apps/common/foo.h",
                "cs/apps/code-viewer/main.cc", "cs/"),
            "cs/apps/common/foo.h");
  EXPECT_EQ(ResolveQuotedIncludePath(
                "cs/net/http/request.hh",
                "cs/apps/code-viewer/ui.gpt.cc", "cs/"),
            "cs/net/http/request.hh");
}

TEST(ResolveQuotedIncludePathTest,
     RepoPrefixEmptyUsesRelativeResolution) {
  EXPECT_EQ(ResolveQuotedIncludePath(
                "cs/apps/common/foo.h",
                "cs/apps/code-viewer/main.cc", ""),
            "cs/apps/code-viewer/cs/apps/common/foo.h");
}

TEST(ResolveQuotedIncludePathTest,
     RepoPrefixNonMatchingUsesRelativeResolution) {
  EXPECT_EQ(ResolveQuotedIncludePath(
                "other/bar.h",
                "cs/apps/code-viewer/main.cc", "cs/"),
            "cs/apps/code-viewer/other/bar.h");
}

// ---- HTML generation
// (RenderCodeLineWithQuotedIncludeLinks) ----

TEST(QuotedIncludeLinkTest, DisabledReturnsEscapedLine) {
  std::string line = "#include \"foo/bar.h\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, false, "a/main.cc", "/");
  EXPECT_THAT(out, HasSubstr("foo/bar.h"));
  EXPECT_THAT(out, Not(HasSubstr("<a ")));
}

TEST(QuotedIncludeLinkTest, QuotedIncludeWithPathResolved) {
  std::string line = "#include \"foo/bar.h\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "a/main.cc", "/");
  EXPECT_THAT(out, HasSubstr("<a href="));
  EXPECT_THAT(out, HasSubstr("a/foo/bar.h"));
  EXPECT_THAT(out, HasSubstr("foo/bar.h"));
}

TEST(QuotedIncludeLinkTest, AngleIncludeNotLinked) {
  std::string line = "#include <vector>";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "a/main.cc", "/");
  EXPECT_THAT(out, Not(HasSubstr("<a href=")));
  EXPECT_THAT(out, HasSubstr("vector"));
}

TEST(QuotedIncludeLinkTest, NoIncludeUnchanged) {
  std::string line = "int x;";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "a/main.cc", "/");
  EXPECT_THAT(out, Not(HasSubstr("<a href=")));
  EXPECT_THAT(out, HasSubstr("int x;"));
}

TEST(QuotedIncludeLinkTest, QuotedIncludeSameDirHref) {
  std::string line = "#include \"h.h\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "dir/file.cc", "/");
  EXPECT_THAT(out, HasSubstr("<a href="));
  EXPECT_THAT(out, HasSubstr("dir/h.h"));
}

TEST(QuotedIncludeLinkTest,
     HtmlHrefExactFormatForRelativeInclude) {
  std::string line = "#include \"foo/bar.h\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "a/main.cc", "/");
  EXPECT_THAT(out, HasSubstr("href=\"/a/foo/bar.h\""));
  EXPECT_THAT(out, HasSubstr(">foo/bar.h<"));
}

TEST(QuotedIncludeLinkTest,
     RepoRelativeIncludeProducesCorrectHrefNoDoubleCs) {
  std::string line =
      "#include \"cs/apps/common/file_viewer.gpt.hh\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "cs/apps/code-viewer/ui.gpt.cc", "/",
      "cs/");
  EXPECT_THAT(
      out,
      HasSubstr(
          "href=\"/cs/apps/common/file_viewer.gpt.hh\""));
  EXPECT_THAT(out, Not(HasSubstr("code-viewer/cs/apps")));
  EXPECT_THAT(
      out,
      HasSubstr(">cs/apps/common/file_viewer.gpt.hh<"));
}

TEST(QuotedIncludeLinkTest,
     WithoutRepoPrefixRepoStylePathDoubledInHref) {
  std::string line = "#include \"cs/apps/common/foo.h\"";
  std::string out = RenderCodeLineWithQuotedIncludeLinks(
      line, true, "cs/apps/code-viewer/main.cc", "/", "");
  EXPECT_THAT(
      out, HasSubstr(
               "cs/apps/code-viewer/cs/apps/common/foo.h"));
}

}  // namespace
