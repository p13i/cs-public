// cs/apps/prober/prober.gpt.hh
#ifndef CS_APPS_PROBER_PROBER_GPT_HH
#define CS_APPS_PROBER_PROBER_GPT_HH

#include <string>
#include <vector>

#include "cs/apps/prober/protos/probes.proto.hh"
#include "cs/result.hh"

namespace cs::apps::prober {

// Executes each probe request via cs::FetchResponse, using
// host and port to construct request URLs. Returns the
// first error or Ok().
::cs::Result RunProbes(
    const std::string& host, int port,
    const std::vector<protos::Probe>& probes);

}  // namespace cs::apps::prober

#endif  // CS_APPS_PROBER_PROBER_GPT_HH
