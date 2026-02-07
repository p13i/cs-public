// cs/apps/prober/protos/probes.proto.hh
#ifndef CS_APPS_PROBER_PROTOS_PROBES_PROTO_HH
#define CS_APPS_PROBER_PROTOS_PROBES_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/json/object.hh"
#include "cs/net/proto/proto.hh"

namespace cs::apps::prober::protos {

// HTTP request part of a probe.
DECLARE_PROTO(ProbeRequest) {
  [[required]] [[enum_in("GET", "POST")]]
  std::string method;
  [[required]]
  std::string url;
  // Optional JSON body for POST (e.g. RPC payload). Use a
  // JSON object in probes.json for readability;
  // null/omitted means no body. Ignored when body_file is
  // set.
  cs::net::json::Object body;
  // Optional: path to file to send as raw body (e.g. audio
  // for scribe-service upload). When set, body is ignored.
  std::string body_file;
  // Content-Type when body_file is set; default
  // application/octet-stream.
  std::string content_type;
};

// Expected response part of a probe.
DECLARE_PROTO(ExpectedResponse) {
  [[required]] [[ge(200), lt(600)]]
  int status;
  // Optional: when set, response body must match (string or
  // JSON).
  cs::net::json::Object body;
};

// Single probe: request and expected response.
DECLARE_PROTO(Probe) {
  [[required]]
  std::string description;
  [[required]]
  ProbeRequest request;
  [[required]]
  ExpectedResponse expected_response;
};

// Root type for probes.json: list of probes.
DECLARE_PROTO(Probes) { std::vector<Probe> probes; };

}  // namespace cs::apps::prober::protos

#endif  // CS_APPS_PROBER_PROTOS_PROBES_PROTO_HH
