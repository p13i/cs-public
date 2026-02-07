// cs/net/rpc/client_test.gpt.cc
#include "cs/net/rpc/client.hh"

#include <string>

#include "cs/net/http/client.hh"
#include "cs/net/http/response.hh"
#include "cs/result.hh"
#include "gtest/gtest.h"

namespace cs::net::rpc {
namespace {

// Minimal proto-like types for testing the template
// instantiation.
struct TestRequest {
  std::string msg;
  std::string Serialize() const {
    return "{\"msg\":\"" + msg + "\"}";
  }
};

struct TestResponse {
  std::string reply;
  TestResponse Parse(const std::string& body) {
    TestResponse r;
    r.reply = body;
    return r;
  }
};

struct TestRPC {
  using RequestType = TestRequest;
  using ResponseType = TestResponse;
};

TEST(RPCClientTest, ConstructsWithBaseUrl) {
  RPCClient<TestRPC> client("http://localhost:9999");
  // No crash on construction; the base URL is stored.
}

TEST(RPCClientTest, CallToUnreachableHostFails) {
  RPCClient<TestRPC> client("http://127.0.0.1:1");
  TestRequest req{.msg = "hello"};
  auto result = client.Call("/test", req);
  // Should fail because nothing is listening on port 1.
  EXPECT_FALSE(result.ok());
}

}  // namespace
}  // namespace cs::net::rpc
