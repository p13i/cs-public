// cs/apps/common/domain_resolver.gpt.hh
#ifndef CS_APPS_COMMON_DOMAIN_RESOLVER_GPT_HH
#define CS_APPS_COMMON_DOMAIN_RESOLVER_GPT_HH

#include <string>

#include "cs/net/http/request.hh"
#include "cs/result.hh"

namespace cs::apps::common {

// Determines environment ("dev" or "prod") from the request
// Host header. Host starting with "dev." yields "dev";
// otherwise "prod". Returns an error if the Host header is
// missing.
cs::ResultOr<std::string> DetermineEnvironment(
    const cs::net::http::Request& request);

// Looks up the domain for the given service name from
// embedded routing.json. Returns the domain (e.g.
// "code.trycopilot.ai" or "dev.code.trycopilot.ai") or an
// error if the service is not in the routing config.
cs::ResultOr<std::string> GetDomainForService(
    const std::string& service_name,
    const cs::net::http::Request& request);

}  // namespace cs::apps::common

#endif  // CS_APPS_COMMON_DOMAIN_RESOLVER_GPT_HH
