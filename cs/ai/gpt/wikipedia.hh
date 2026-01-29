// cs/ai/gpt/wikipedia.hh
#ifndef CS_AI_GPT_WIKIPEDIA_HH
#define CS_AI_GPT_WIKIPEDIA_HH

#include <string>
#include <vector>

#include "cs/result.hh"

namespace cs::ai::gpt {
cs::ResultOr<std::vector<std::string>>
DownloadMostPopularArticles(unsigned int n);
}

#endif  // CS_AI_GPT_WIKIPEDIA_HH