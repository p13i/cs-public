// cs/apps/prober/prober.gpt.cc
#include "cs/apps/prober/prober.gpt.hh"

#include <map>
#include <string>

#include "cs/apps/prober/protos/probes.proto.hh"
#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/client.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/result.hh"
#include "cs/util/fmt.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::FetchResponse;
using ::cs::Ok;
using ::cs::Result;
using ::cs::net::json::Type;
using ::cs::net::json::parsers::ParseObject;
}  // namespace

namespace cs::apps::prober {

Result RunProbes(const std::string& host, int port,
                 const std::vector<protos::Probe>& probes) {
  LOG(INFO) << "Running " << probes.size()
            << " probes against " << host << ":" << port
            << ENDL;
  const std::map<std::string, std::string> no_headers;
  std::string base =
      "http://" + host + ":" + std::to_string(port);
  int failed = 0;
  for (const auto& probe : probes) {
    std::string path = probe.request.url;
    if (path.empty() || path[0] != '/') {
      path = "/" + path;
    }
    std::string url = base + path;
    LOG(INFO) << "Running probe against "
              << probe.request.method << " " << url << ENDL;
    std::map<std::string, std::string> headers = no_headers;
    std::string body_str;
    if (!probe.request.body_file.empty()) {
      auto read_or =
          ::cs::fs::read(probe.request.body_file);
      if (!read_or.ok()) {
        LOG(WARNING) << "[FAIL] probe "
                     << probe.request.method << " " << url
                     << ": body_file "
                     << probe.request.body_file << ": "
                     << read_or.message() << ENDL;
        failed++;
        continue;
      }
      body_str = std::move(read_or).value();
      headers["Content-Type"] =
          probe.request.content_type.empty()
              ? "application/octet-stream"
              : probe.request.content_type;
    } else if (probe.request.body.type() !=
               ::cs::net::json::Type::kNull) {
      body_str = probe.request.body.str();
      headers["Content-Type"] = "application/json";
    }
    auto response = FetchResponse(url, probe.request.method,
                                  headers, body_str);
    if (!response.ok()) {
      LOG(WARNING) << "[FAIL] probe "
                   << probe.request.method << " " << url
                   << ": " << response.message() << ENDL;
      failed++;
      continue;
    }
    if (static_cast<int>(response.value().status().code) !=
        probe.expected_response.status) {
      LOG(WARNING) << FMT(
          "[FAIL] probe %s %s: expected status %d, got %u",
          probe.request.method.c_str(), url.c_str(),
          probe.expected_response.status,
          response.value().status().code);
      failed++;
      continue;
    }
    if (probe.expected_response.body.type() !=
        ::cs::net::json::Type::kNull) {
      auto parsed = ParseObject(response.value().body());
      if (!parsed.ok()) {
        LOG(WARNING)
            << "[FAIL] probe " << probe.request.method
            << " " << url
            << ": response body is not valid JSON: "
            << parsed.message() << ENDL;
        failed++;
        continue;
      }
      if (parsed.value() != probe.expected_response.body) {
        LOG(WARNING)
            << "[FAIL] probe " << probe.request.method
            << " " << url
            << ": response body does not match expected"
            << ENDL;
        LOG(WARNING) << "expected: "
                     << probe.expected_response.body.str()
                     << ENDL;
        LOG(WARNING) << "actual: "
                     << response.value().body() << ENDL;
        failed++;
        continue;
      }
    }
    LOG(INFO) << "[OK] probe " << probe.request.method
              << " " << url << " "
              << probe.expected_response.status << " ok"
              << ENDL;
  }
  if (failed != 0) {
    return Error(FMT("%d probe(s) failed", failed));
  }
  return Ok();
}

}  // namespace cs::apps::prober
