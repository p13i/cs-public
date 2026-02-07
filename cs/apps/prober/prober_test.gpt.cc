// cs/apps/prober/prober_test.gpt.cc
#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "cs/apps/prober/prober.gpt.hh"
#include "cs/apps/prober/protos/probes.proto.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::apps::prober::RunProbes;
using ::cs::apps::prober::protos::Probe;
using ::cs::apps::prober::protos::Probes;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::Server;
using ::testing::Eq;
using ::testing::HasSubstr;
}  // namespace

namespace {

const char* kTestHost = "127.0.0.1";
const int kProberTestPort = 31950;
const int kServerStartupMs = 500;

void StartProberTestServer(Server* server) {
  server->StartListening([](Request req) {
    if (req.path() == "/" || req.path().empty()) {
      return HtmlResponse("ok");
    }
    if (req.path() == "/health" ||
        req.path() == "/health/") {
      return HtmlResponse("ok");
    }
    if (req.path() == "/status404") {
      return Response(HTTP_404_NOT_FOUND, "text/plain",
                      "not found");
    }
    if (req.path() == "/echo") {
      return Response(HTTP_200_OK, "text/plain",
                      req.body());
    }
    if (req.path() == "/json") {
      return Response(HTTP_200_OK, "application/json",
                      "{\"a\":1,\"b\":2}");
    }
    if (req.path() == "/badjson") {
      return Response(HTTP_200_OK, "text/plain",
                      "not valid json");
    }
    if (req.path() == "/json-mismatch") {
      return Response(HTTP_200_OK, "application/json",
                      "{\"x\":99}");
    }
    return HtmlResponse("ok");
  });
}

std::vector<Probe> ParseProbes(const std::string& json) {
  Probes root;
  auto parsed = root.Parse(json);
  if (!parsed.ok()) {
    return {};
  }
  return parsed.value().probes;
}

class ProberTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    server_ = std::make_unique<Server>(kTestHost,
                                       kProberTestPort);
    ASSERT_OK(server_->Bind());
    server_thread_ = std::make_unique<std::thread>(
        StartProberTestServer, server_.get());
    std::this_thread::sleep_for(
        std::chrono::milliseconds(kServerStartupMs));
  }

  static void TearDownTestSuite() {
    server_thread_->detach();
    server_.reset();
  }

  static std::unique_ptr<Server> server_;
  static std::unique_ptr<std::thread> server_thread_;
};

std::unique_ptr<Server> ProberTest::server_;
std::unique_ptr<std::thread> ProberTest::server_thread_;

}  // namespace

// -----------------------------------------------------------------------------
// Empty and single-probe success
// -----------------------------------------------------------------------------

TEST_F(ProberTest, RunProbes_EmptyProbes_ReturnsOk) {
  auto result = RunProbes(kTestHost, kProberTestPort, {});
  ASSERT_OK(result);
}

TEST_F(ProberTest, RunProbes_SingleGet_200_ReturnsOk) {
  std::string probes_json = R"({
    "probes": [{
      "description": "GET root",
      "request": {"method": "GET", "url": "/"},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

// -----------------------------------------------------------------------------
// Status code failure
// -----------------------------------------------------------------------------

TEST_F(ProberTest, RunProbes_SingleGet_404_ReturnsError) {
  std::string probes_json = R"({
    "probes": [{
      "description": "Expect 200 get 404",
      "request": {"method": "GET", "url": "/status404"},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("1 probe(s) failed"));
}

// -----------------------------------------------------------------------------
// Path normalization: empty path and path without leading
// slash
// -----------------------------------------------------------------------------

TEST_F(ProberTest, RunProbes_PathEmpty_PrefixesSlash) {
  std::string probes_json = R"({
    "probes": [{
      "description": "Empty path becomes /",
      "request": {"method": "GET", "url": ""},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

TEST_F(ProberTest,
       RunProbes_PathNoLeadingSlash_PrefixesSlash) {
  std::string probes_json = R"({
    "probes": [{
      "description": "health without leading slash",
      "request": {"method": "GET", "url": "health"},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

// -----------------------------------------------------------------------------
// POST with body (no expected body)
// -----------------------------------------------------------------------------

TEST_F(ProberTest, RunProbes_PostWithBody_ReturnsOk) {
  std::string probes_json = R"({
    "probes": [{
      "description": "POST echo",
      "request": {
        "method": "POST",
        "url": "/echo",
        "body": {"key": "value"}
      },
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

// -----------------------------------------------------------------------------
// Expected response body: match, mismatch, invalid JSON
// -----------------------------------------------------------------------------

TEST_F(ProberTest,
       RunProbes_ExpectedBody_Matches_ReturnsOk) {
  std::string probes_json = R"({
    "probes": [{
      "description": "JSON body match",
      "request": {"method": "GET", "url": "/json"},
      "expected_response": {
        "status": 200,
        "body": {"a": 1, "b": 2}
      }
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

TEST_F(ProberTest,
       RunProbes_ExpectedBody_Mismatch_ReturnsError) {
  std::string probes_json = R"({
    "probes": [{
      "description": "JSON body mismatch",
      "request": {"method": "GET", "url": "/json-mismatch"},
      "expected_response": {
        "status": 200,
        "body": {"a": 1}
      }
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("1 probe(s) failed"));
}

TEST_F(ProberTest,
       RunProbes_ExpectedBody_InvalidJson_ReturnsError) {
  std::string probes_json = R"({
    "probes": [{
      "description": "Response not valid JSON",
      "request": {"method": "GET", "url": "/badjson"},
      "expected_response": {
        "status": 200,
        "body": {"a": 1}
      }
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("1 probe(s) failed"));
}

// -----------------------------------------------------------------------------
// Multiple probes
// -----------------------------------------------------------------------------

TEST_F(ProberTest,
       RunProbes_MultipleProbes_AllPass_ReturnsOk) {
  std::string probes_json = R"({
    "probes": [
      {
        "description": "First",
        "request": {"method": "GET", "url": "/"},
        "expected_response": {"status": 200}
      },
      {
        "description": "Second",
        "request": {"method": "GET", "url": "/health"},
        "expected_response": {"status": 200}
      }
    ]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(2u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}

TEST_F(ProberTest,
       RunProbes_MultipleProbes_OneFails_ReturnsError) {
  std::string probes_json = R"({
    "probes": [
      {
        "description": "Pass",
        "request": {"method": "GET", "url": "/"},
        "expected_response": {"status": 200}
      },
      {
        "description": "Fail status",
        "request": {"method": "GET", "url": "/status404"},
        "expected_response": {"status": 200}
      }
    ]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(2u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("1 probe(s) failed"));
}

TEST_F(ProberTest,
       RunProbes_MultipleProbes_TwoFail_ReturnsError) {
  std::string probes_json = R"({
    "probes": [
      {
        "description": "Fail 404",
        "request": {"method": "GET", "url": "/status404"},
        "expected_response": {"status": 200}
      },
      {
        "description": "Fail 404 again",
        "request": {"method": "GET", "url": "/status404"},
        "expected_response": {"status": 200}
      }
    ]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(2u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("2 probe(s) failed"));
}

// -----------------------------------------------------------------------------
// Unreachable host (FetchResponse fails)
// -----------------------------------------------------------------------------

TEST_F(ProberTest, RunProbes_UnreachableHost_ReturnsError) {
  std::string probes_json = R"({
    "probes": [{
      "description": "Unreachable",
      "request": {"method": "GET", "url": "/"},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result = RunProbes("127.0.0.1", 1, probes);
  EXPECT_NOK(result);
  EXPECT_THAT(result.message(),
              HasSubstr("1 probe(s) failed"));
}

// -----------------------------------------------------------------------------
// Expected response body null: only status checked
// -----------------------------------------------------------------------------

TEST_F(ProberTest,
       RunProbes_ExpectedBodyOmitted_OnlyStatusChecked) {
  std::string probes_json = R"({
    "probes": [{
      "description": "No body expectation",
      "request": {"method": "GET", "url": "/json"},
      "expected_response": {"status": 200}
    }]
  })";
  std::vector<Probe> probes = ParseProbes(probes_json);
  ASSERT_THAT(probes.size(), Eq(1u));

  auto result =
      RunProbes(kTestHost, kProberTestPort, probes);
  ASSERT_OK(result);
}
