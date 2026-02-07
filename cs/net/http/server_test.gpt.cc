// cs/net/http/server_test.gpt.cc
#include "cs/net/http/server.hh"

#include <chrono>
#include <functional>
#include <string>
#include <thread>

#include "cs/net/html/dom.hh"
#include "cs/net/http/client.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::FetchResponse;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::Server;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::IsTrue;
using ::testing::Le;
}  // namespace

namespace {

const char* kTestHost = "127.0.0.1";
const int kBasePort = 31910;
const int kServerStartupMs = 200;

}  // namespace

// -----------------------------------------------------------------------------
// Bind and Close
// -----------------------------------------------------------------------------

TEST(ServerTest, BindSucceedsOnLocalhost) {
  Server server(kTestHost, kBasePort);
  auto result = server.Bind();
  ASSERT_TRUE(result.ok()) << result.message();
}

TEST(ServerTest, CloseAfterBindSucceeds) {
  Server server(kTestHost, kBasePort + 1);
  EXPECT_OK(server.Bind());
  ASSERT_OK(server.Close());
}

TEST(ServerTest, BindDifferentPortsSucceeds) {
  Server server1(kTestHost, kBasePort + 2);
  Server server2(kTestHost, kBasePort + 3);
  ASSERT_OK(server1.Bind());
  ASSERT_OK(server2.Bind());
  ASSERT_OK(server1.Close());
  ASSERT_OK(server2.Close());
}

// -----------------------------------------------------------------------------
// GetThreadLoad
// -----------------------------------------------------------------------------

TEST(ServerTest, GetThreadLoadZeroBeforeStartListening) {
  Server server(kTestHost, kBasePort + 4, 10);
  ASSERT_OK(server.Bind());
  float load = server.GetThreadLoad();
  EXPECT_THAT(load, Ge(0.0f));
  EXPECT_THAT(load, Le(1.0f));
  EXPECT_THAT(load, Eq(0.0f));
  ASSERT_OK(server.Close());
}

TEST(ServerTest, GetThreadLoadZeroWhenThreadPoolSizeZero) {
  Server server(kTestHost, kBasePort + 5, 0);
  ASSERT_OK(server.Bind());
  EXPECT_THAT(server.GetThreadLoad(), Eq(0.0f));
  ASSERT_OK(server.Close());
}

TEST(ServerTest, GetThreadLoadSingleThreadPool) {
  Server server(kTestHost, kBasePort + 6, 1);
  ASSERT_OK(server.Bind());
  EXPECT_THAT(server.GetThreadLoad(), Eq(0.0f));
  server.Close();
}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------

TEST(ServerTest, ConstructorTwoArg) {
  Server server(kTestHost, kBasePort + 7);
  ASSERT_OK(server.Bind());
  ASSERT_OK(server.Close());
}

TEST(ServerTest, ConstructorThreeArgThreadPoolSize) {
  Server server(kTestHost, kBasePort + 8, 4);
  ASSERT_OK(server.Bind());
  ASSERT_OK(server.Close());
}

// -----------------------------------------------------------------------------
// Integration: Bind, StartListening in thread, handle
// request via Fetch
// -----------------------------------------------------------------------------

TEST(ServerTest,
     StartListeningAcceptsConnectionAndHandlerRuns) {
  Server server(kTestHost, kBasePort + 9);
  ASSERT_OK(server.Bind());
  std::thread listener([&server]() {
    server.StartListening([](Request req) {
      return HtmlResponse(cs::net::html::dom::p("handled"));
    });
  });
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto response = FetchResponse("http://127.0.0.1:31919/",
                                "GET", {}, "");
  ASSERT_OK(response);
  EXPECT_THAT(response.value().status().code, Eq(200u));
  EXPECT_THAT(response.value().body(),
              Eq(cs::net::html::dom::p("handled")));

  listener.detach();
}
