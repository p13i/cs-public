// cs/apps/common/file-viewer/quoted_include_link.gpt.hh
#ifndef CS_APPS_COMMON_FILE_VIEWER_QUOTED_INCLUDE_LINK_GPT_HH
#define CS_APPS_COMMON_FILE_VIEWER_QUOTED_INCLUDE_LINK_GPT_HH

#include <string>

namespace cs::apps::common::file_viewer {

// Resolves the path from #include "path" to the
// viewer-relative path used for the link. When
// repo_include_prefix is non-empty and path starts with it,
// path is used as-is (repo-root-relative); otherwise path
// is resolved relative to the current file's directory.
std::string ResolveQuotedIncludePath(
    const std::string& quoted_include_path,
    const std::string& current_file_relative_path,
    const std::string& repo_include_prefix);

// Renders a single line of code. When link_quoted_includes
// is true and the line matches #include "path", the path is
// emitted as a link (using the HTML DOM DSL) to the
// resolved path; angle form #include <path> is not linked.
// repo_include_prefix: if non-empty, paths starting with
// this (e.g. "cs/") are treated as repo-root-relative.
std::string RenderCodeLineWithQuotedIncludeLinks(
    const std::string& line, bool link_quoted_includes,
    const std::string& relative_path,
    const std::string& normalized_base,
    const std::string& repo_include_prefix = "");

}  // namespace cs::apps::common::file_viewer

#endif  // CS_APPS_COMMON_FILE_VIEWER_QUOTED_INCLUDE_LINK_GPT_HH
