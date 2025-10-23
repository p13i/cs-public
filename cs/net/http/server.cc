
#include "cs/net/http/server.hh"

#include <unistd.h>

#include <iostream>
#include <sstream>

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/parsers/parsers.hh"
#include "cs/result.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"

// Based on: https://github.com/OsasAzamegbe/http-server
namespace {

const unsigned int BUFFER_SIZE = 2 << 15;
#define VERBOSE_LOG true

using ::cs::Error;
using ::cs::Ok;
using ::cs::Result;
using ::cs::parsers::ParseInt;

}  // namespace

namespace cs::net::http {

Server::Server(std::string ip_address, int port)
    : _ip_address(ip_address),
      _port(port),
      _socket_address_length(sizeof(_socket_address)),
      uuid_(cs::util::random::uuid::generate_uuid_v4()) {
  _socket_address.sin_family = AF_INET;
  _socket_address.sin_port = htons(_port);
  _socket_address.sin_addr.s_addr =
      inet_addr(_ip_address.c_str());
}

Server::~Server() { Close(); }

Result Server::Bind() {
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0) {
    return TRACE(Error("Failed to get socket."));
  }

  if (bind(_socket, (sockaddr *)&_socket_address,
           _socket_address_length) < 0) {
    return TRACE(Error("Failed to bind to socket."));
  }

  return Ok();
}

Result Server::Close() {
  close(_socket);
  close(_response_socket);
  return Ok();
}

Result Server::StartListening(
    std::function<Response(Request)> request_handler) {
  if (listen(_socket, 20) < 0) {
    return TRACE(Error("Failed to listen on socket."));
  }

  LOG(INFO) << "Listening on "
            << inet_ntoa(_socket_address.sin_addr) << ":"
            << ntohs(_socket_address.sin_port) << ENDL;

  Request request;
  Response response;
  Result result = Error("unknown state");
  while (true) {
    _response_socket =
        accept(_socket, (sockaddr *)&_socket_address,
               &_socket_address_length);

    [[maybe_unused]] unsigned int processing_time_ms =
        cs::util::timeit([&]() {
          if (_response_socket < 0) {
            result = Error("Failed to accept connection.");
            return;
          }

          char buffer[BUFFER_SIZE] = {0};
          int bytesReceived =
              read(_response_socket, buffer, BUFFER_SIZE);

          if (bytesReceived < 0) {
            result = Error("Failed to read from socket.");
            return;
          }

          request = Request();
          Result parse_result = buffer >> request;
          if (!parse_result.ok()) {
            result = parse_result;
            return;
          }

#if VERBOSE_LOG
          LOG(INFO) << ">>> " << request.summary() << ENDL;
#endif  // VERBOSE_LOG

          response = request_handler(request);

          std::string response_str = response.to_string();
          // stop timer
          unsigned long int bytesSent =
              write(_response_socket, response_str.c_str(),
                    response_str.size());

          if (bytesSent != response_str.size()) {
            LOG(ERROR) << "Failed to send out " << bytesSent
                       << "." << ENDL;
            return;
          }

          LOG(INFO) << "<<< " << response.status() << " "
                    << response._result << " "
                    << response.body().size() << " bytes"
                    << ENDL;

          result = Ok();
        });
    // end cs::util::timeit

    LOG(DEBUG) << "[" << request._method << " "
               << request._path
               << "] Processed HTTP request in "
               << processing_time_ms << "ms: " << result
               << ENDL;

    close(_response_socket);
  }

  return Ok();
}

}  // namespace cs::net::http
