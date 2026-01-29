// cs/ai/gpt/download_wikipedia.cc
#include <filesystem>
#include <iostream>
#include <regex>

#include "cs/ai/gpt/wikipedia.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

cs::Result DownloadWikipedia(
    unsigned int n, const std::string& out_dir_str) {
  std::filesystem::path out_dir(out_dir_str);
  std::filesystem::create_directories(out_dir);

  SET_OR_RET(auto articles,
             cs::ai::gpt::DownloadMostPopularArticles(n));

  for (size_t i = 0; i < articles.size(); ++i) {
    const std::string& html = articles[i];

    // Extract title from <title> tag
    std::smatch match;
    std::string title =
        "article_" + std::to_string(i + 1);  // fallback
    if (std::regex_search(
            html, match,
            std::regex("<title>([^<]+)</title>"))) {
      title = match[1].str();
      std::transform(title.begin(), title.end(),
                     title.begin(), ::tolower);
      std::replace(title.begin(), title.end(), ' ', '_');
      std::replace(title.begin(), title.end(), '/', '_');
      std::replace(title.begin(), title.end(), ':', '_');
    }

    std::filesystem::path path =
        out_dir / (title + ".html");
    LOG(INFO) << "Writing article to " << path.string()
              << " starting with: \"" << html.substr(0, 30)
              << "...\"" << ENDL;
    OK_OR_RETURN(cs::fs::write(path.string(), html));
    LOG(INFO) << "Wrote " << path.string() << ENDL;
  }

  return cs::Ok("Downloaded " +
                std::to_string(articles.size()) +
                " articles to " + out_dir.string());
}

int main(int argc, char** argv) {
  return cs::Result::Main(
      argc, argv,
      [](std::vector<std::string> args) -> cs::Result {
        if (args.size() != 3) {
          return TRACE(cs::Error(
              FMT("Invalid arguments. Usage: ", args[0],
                  " <N> <OUT_DIR>")));
        }
        SET_OR_RET(unsigned int n,
                   cs::parsers::ParseUnsignedInt(args[1]));
        std::filesystem::path out_dir(args[2]);
        return DownloadWikipedia(n, out_dir.string());
      });
}
