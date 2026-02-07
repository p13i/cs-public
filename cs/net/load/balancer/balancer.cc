// cs/net/load/balancer/balancer.cc
#include "cs/net/load/balancer/balancer.hh"

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "cs/net/html/dom.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/status.hh"
#include "cs/util/fmt.hh"
#include "cs/util/random.hh"
#include "cs/util/timeit.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::Fetch;
using ::cs::ResultOr;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::load::balancer::protos::DownstreamClient;
}  // namespace

namespace cs::net::load::balancer {

ResultOr<DownstreamClient> PickLeastLoaded(
    const std::vector<DownstreamClient>& clients) {
  if (clients.empty()) {
    return TRACE(Error("No downstream clients available."));
  }

  // Find the minimum load.
  float min_load = clients.front().load;
  for (const auto& c : clients) {
    if (c.load < min_load) {
      min_load = c.load;
    }
  }

  // Collect all clients with the minimum load.
  std::vector<size_t> candidates;
  for (size_t i = 0; i < clients.size(); ++i) {
    if (clients[i].load == min_load) {
      candidates.push_back(i);
    }
  }

  // Randomly select among candidates with equal load.
  size_t index =
      cs::util::random::randomInt(0, candidates.size() - 1);
  return clients[candidates[index]];
}

// Helper to build a query string from the request's query
// params.
static std::string BuildQueryString(
    const std::map<std::string, std::string>& params) {
  if (params.empty()) return "";
  std::stringstream ss;
  bool first = true;
  for (const auto& [k, v] : params) {
    if (!first) ss << "&";
    first = false;
    ss << k << "=" << v;
  }
  return ss.str();
}

// Helper to insert load balancer message into the Footer
// component. Finds the marker element and replaces it with
// the marker plus the load balancer message.
static std::string InsertLoadBalancerMessageIntoFooter(
    const std::string& html_body,
    const std::string& message) {
  using namespace cs::net::html::dom;

  // Construct the marker span using the dom library, same
  // as in MakeFooterComponent().
  const std::string marker =
      span({{"id", "load-balancer-here"}}, "");

  // Find the marker in the HTML body.
  size_t pos = html_body.find(marker);
  if (pos == std::string::npos) {
    // Marker not found, fall back to appending.
    return html_body + message;
  }

  // Replace the marker with marker + message.
  std::string result = html_body;
  result.replace(pos, marker.size(), marker + message);
  return result;
}

Response ProxyToDownstream(
    const Request& request,
    const DownstreamClient& downstream) {
  const auto query =
      BuildQueryString(request._query_params);
  std::string url;
  if (query.empty()) {
    url = FMT("http://%s:%d%s", downstream.host.c_str(),
              downstream.port, request._path.c_str());
  } else {
    url = FMT("http://%s:%d%s?%s", downstream.host.c_str(),
              downstream.port, request._path.c_str(),
              query.c_str());
  }

  auto [downstream_response_or,
        proxy_runtime_ms] =  // LCOV_EXCL_LINE
      cs::util::timeit<ResultOr<Response>>(
          [&]() {  // LCOV_EXCL_LINE
            return FetchResponse(url, request._method,
                                 request._headers,
                                 request._body);
          });

  if (!downstream_response_or.ok()) {
    return Response(
        HTTP_500_INTERNAL_SERVER_ERROR,
        cs::net::http::kContentTypeTextPlain,
        FMT("Failed to fetch response from downstream: %s",
            downstream_response_or.message().c_str()));
  }

  auto downstream_response = downstream_response_or.value();

  Response response(downstream_response._status,
                    downstream_response._content_type,
                    downstream_response._body);
  response._headers = downstream_response._headers;

  // If the response is HTML, insert the internal server
  // address into the Footer component.
  if (response._content_type ==
      cs::net::http::kContentTypeTextHtml) {
    // Remove Content-Length header since we're modifying
    // the body.
    response._headers.erase("Content-Length");
    using namespace cs::net::html::dom;
    std::string load_balancer_message =
        nbsp() + "|" + nbsp() +
        "Load balanced to:" + nbsp() +
        code(FMT("%s:%d", downstream.host.c_str(),
                 downstream.port)) +
        nbsp() + FMT("in %dms.", proxy_runtime_ms);
    response._body = InsertLoadBalancerMessageIntoFooter(
        response._body, load_balancer_message);
  }

  return response;
}

}  // namespace cs::net::load::balancer