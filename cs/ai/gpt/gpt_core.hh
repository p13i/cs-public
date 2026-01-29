// cs/ai/gpt/gpt_core.hh
#ifndef CS_AI_GPT_GPT_CORE_HH
#define CS_AI_GPT_GPT_CORE_HH

#include <string>

#include "cs/result.hh"

// Core API for the local ASCII GPT model. These functions
// are implemented in gpt/main.cc and can be used both by
// the CLI binary and HTTP adapters.
bool EnsureModelReady();
std::string GptEvaluate(std::string user_input);
cs::Result TrainPopularWikipediaModel(size_t target_count,
                                      size_t min_success);

#endif  // CS_AI_GPT_GPT_CORE_HH
