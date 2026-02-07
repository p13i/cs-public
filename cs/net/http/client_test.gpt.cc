// cs/net/http/client_test.gpt.cc
#include "cs/net/http/client.hh"

#include <chrono>
#include <map>
#include <string>
#include <thread>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/result.hh"
#include "cs/testing/extensions.gpt.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::Fetch;
using ::cs::FetchResponse;
using ::cs::net::http::HtmlResponse;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::Server;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::IsFalse;
using ::testing::IsTrue;
}  // namespace

namespace {

const char* kTestHost = "127.0.0.1";
const int kTestPort = 31902;
const int kServerStartupMs = 300;

void StartServerInBackground(Server* server) {
  server->StartListening([](Request req) {
    if (req.path() == "/echo") {
      return Response(HTTP_200_OK, "text/plain",
                      req.body());
    }
    if (req.path() == "/status404") {
      return Response(HTTP_404_NOT_FOUND, "text/plain",
                      "not found");
    }
    return HtmlResponse("ok");
  });
}

}  // namespace

// -----------------------------------------------------------------------------
// Fetch (body only) against local server
// -----------------------------------------------------------------------------

TEST(ClientTest, FetchGetReturnsBody) {
  Server server(kTestHost, kTestPort);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto body =
      Fetch("http://127.0.0.1:31902/", "GET", {}, "");
  ASSERT_OK(body);
  EXPECT_THAT(body.value(), Eq("ok"));

  server_thread.detach();
}

TEST(ClientTest, FetchGetWithPath) {
  Server server(kTestHost, kTestPort + 1);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto body =
      Fetch("http://127.0.0.1:31903/path", "GET", {}, "");
  ASSERT_OK(body);
  EXPECT_THAT(body.value(), Eq("ok"));

  server_thread.detach();
}

TEST(ClientTest, FetchPostWithBody) {
  Server server(kTestHost, kTestPort + 2);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto body = Fetch("http://127.0.0.1:31904/echo", "POST",
                    {}, "hello");
  ASSERT_OK(body);
  EXPECT_THAT(body.value(), Eq("hello"));

  server_thread.detach();
}

TEST(ClientTest, FetchWithCustomHeaders) {
  Server server(kTestHost, kTestPort + 3);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  std::map<std::string, std::string> headers;
  headers["X-Custom"] = "value";
  auto body =
      Fetch("http://127.0.0.1:31905/", "GET", headers, "");
  ASSERT_OK(body);
  EXPECT_THAT(body.value(), Eq("ok"));

  server_thread.detach();
}

// -----------------------------------------------------------------------------
// FetchResponse (full response) against local server
// -----------------------------------------------------------------------------

TEST(ClientTest, FetchResponseReturnsStatusAndBody) {
  Server server(kTestHost, kTestPort + 4);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto response = FetchResponse("http://127.0.0.1:31906/",
                                "GET", {}, "");
  ASSERT_OK(response);
  EXPECT_THAT(response.value().status().code, Eq(200u));
  EXPECT_THAT(response.value().body(), Eq("ok"));

  server_thread.detach();
}

TEST(ClientTest, FetchResponsePostEchoBody) {
  Server server(kTestHost, kTestPort + 5);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto response = FetchResponse(
      "http://127.0.0.1:31907/echo", "POST", {}, "payload");
  ASSERT_OK(response);
  EXPECT_THAT(response.value().status().code, Eq(200u));
  EXPECT_THAT(response.value().body(), Eq("payload"));

  server_thread.detach();
}

TEST(ClientTest, FetchResponseContentType) {
  Server server(kTestHost, kTestPort + 6);
  ASSERT_OK(server.Bind());
  std::thread server_thread(StartServerInBackground,
                            &server);
  std::this_thread::sleep_for(
      std::chrono::milliseconds(kServerStartupMs));

  auto response = FetchResponse("http://127.0.0.1:31908/",
                                "GET", {}, "");
  ASSERT_OK(response);
  EXPECT_THAT(response.value()._content_type,
              Eq("text/html"));

  server_thread.detach();
}

// -----------------------------------------------------------------------------
// Fetch failure (invalid URL / unreachable)
// -----------------------------------------------------------------------------

TEST(ClientTest, FetchInvalidUrlFails) {
  auto body = Fetch("http://127.0.0.1:1/", "GET", {}, "");
  EXPECT_NOK(body);
  EXPECT_THAT(body.message().empty(), IsFalse());
}

TEST(ClientTest, FetchResponseUnreachableFails) {
  auto response =
      FetchResponse("http://127.0.0.1:1/", "GET", {}, "");
  EXPECT_NOK(response);
}
