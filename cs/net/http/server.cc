// cs/net/http/server.cc

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
    : Server(ip_address, port, 10) {}

Server::Server(std::string ip_address, int port,
               size_t thread_pool_size)
    : _ip_address(ip_address),
      _port(port),
      _socket(-1),
      _response_socket(-1),
      _socket_address_length(sizeof(_socket_address)),
      uuid_(cs::util::random::uuid::generate_uuid_v4()),
      _thread_pool_size(thread_pool_size),
      _active_threads(0),
      _shutdown(false) {
  _socket_address.sin_family = AF_INET;
  _socket_address.sin_port = htons(_port);
  _socket_address.sin_addr.s_addr =
      inet_addr(_ip_address.c_str());
}

Server::~Server() {
  _shutdown = true;
  Close();
}

Result Server::Bind() {
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0) {
    return TRACE(Error("Failed to get socket."));
  }

  if (bind(_socket, (sockaddr*)&_socket_address,
           _socket_address_length) < 0) {
    return TRACE(Error("Failed to bind to socket."));
  }

  return Ok();
}

Result Server::Close() {
  StopWorkerThreads();
  if (_socket >= 0) {
    close(_socket);
  }
  if (_response_socket >= 0) {
    close(_response_socket);
  }
  return Ok();
}

float Server::GetThreadLoad() const {
  if (_thread_pool_size == 0) {
    return 0.0f;
  }
  return static_cast<float>(_active_threads.load(
             std::memory_order_relaxed)) /
         static_cast<float>(_thread_pool_size);
}

void Server::WorkerThread(
    std::function<Response(Request)> request_handler) {
  while (true) {
    int client_socket = -1;
    {
      std::unique_lock<std::mutex> lock(_queue_mutex);
      _queue_condition.wait(lock, [this] {
        return !_connection_queue.empty() || _shutdown;
      });

      if (_shutdown && _connection_queue.empty()) {
        return;
      }

      if (!_connection_queue.empty()) {
        client_socket = _connection_queue.front();
        _connection_queue.pop();
      }
    }

    if (client_socket < 0) {
      continue;
    }

    // Increment active thread counter
    _active_threads.fetch_add(1, std::memory_order_relaxed);

    Request request;
    Response response;
    Result result = Error("unknown state");

    [[maybe_unused]]
    unsigned int processing_time_ms =
        cs::util::timeit([&]() {
          char buffer[BUFFER_SIZE] = {0};
          int bytesReceived =
              read(client_socket, buffer, BUFFER_SIZE);

          if (bytesReceived < 0) {
            result = Error("Failed to read from socket.");
            close(client_socket);
            return;
          }

          request = Request();
          Result parse_result = buffer >> request;
          if (!parse_result.ok()) {
            result = parse_result;
            close(client_socket);
            return;
          }

#if VERBOSE_LOG
          LOG(INFO) << ">>> " << request.summary() << ENDL;
#endif  // VERBOSE_LOG

          response = request_handler(request);

          std::string response_str = response.to_string();
          unsigned long int bytesSent =
              write(client_socket, response_str.c_str(),
                    response_str.size());

          if (bytesSent != response_str.size()) {
            LOG(ERROR) << "Failed to send out " << bytesSent
                       << "." << ENDL;
            close(client_socket);
            return;
          }

          LOG(INFO) << "<<< " << response.status() << " "
                    << response._result << " "
                    << response.body().size() << " bytes"
                    << ENDL;

          result = Ok();
        });

    // Decrement active thread counter
    _active_threads.fetch_sub(1, std::memory_order_relaxed);

    LOG(DEBUG) << "[" << request._method << " "
               << request._path
               << "] Processed HTTP request in "
               << processing_time_ms << "ms: " << result
               << ENDL;

    close(client_socket);
  }
}

void Server::StopWorkerThreads() {
  {
    std::lock_guard<std::mutex> lock(_queue_mutex);
    _shutdown = true;
  }
  _queue_condition.notify_all();

  for (auto& thread : _worker_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  _worker_threads.clear();
}

Result Server::StartListening(
    std::function<Response(Request)> request_handler) {
  if (listen(_socket, 20) < 0) {
    return TRACE(Error("Failed to listen on socket."));
  }

  LOG(INFO) << "Listening on "
            << inet_ntoa(_socket_address.sin_addr) << ":"
            << ntohs(_socket_address.sin_port) << " with "
            << _thread_pool_size << " worker threads"
            << ENDL;

  // Start worker threads
  _shutdown = false;
  for (size_t i = 0; i < _thread_pool_size; ++i) {
    _worker_threads.emplace_back(&Server::WorkerThread,
                                 this, request_handler);
  }

  // Main accept loop
  while (true) {
    struct sockaddr_in client_address;
    unsigned int client_address_length =
        sizeof(client_address);
    int client_socket =
        accept(_socket, (sockaddr*)&client_address,
               &client_address_length);

    if (client_socket < 0) {
      LOG(ERROR) << "Failed to accept connection." << ENDL;
      continue;
    }

    // Enqueue the connection for a worker thread to handle
    {
      std::lock_guard<std::mutex> lock(_queue_mutex);
      _connection_queue.push(client_socket);
    }
    _queue_condition.notify_one();
  }

  return Ok();
}

}  // namespace cs::net::http
