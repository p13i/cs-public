// cs/net/http/server.hh
#ifndef CS_NET_HTTP_SERVER_HH
#define CS_NET_HTTP_SERVER_HH

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/result.hh"

namespace cs::net::http {

// Based on: https://github.com/OsasAzamegbe/http-server
class Server {
 public:
  Server(std::string ip_address, int port);
  Server(std::string ip_address, int port,
         size_t thread_pool_size);
  ~Server();
  Result StartListening(
      std::function<Response(Request)> request_handler);

  Result Bind();
  Result Close();

  // Get current thread pool load as ratio of active threads
  // to total threads
  float GetThreadLoad() const;

 private:
  void WorkerThread(
      std::function<Response(Request)> request_handler);
  void StopWorkerThreads();

  std::string _ip_address;
  int _port;
  int _socket;
  int _response_socket;
  struct sockaddr_in _socket_address;
  unsigned int _socket_address_length;
  std::string uuid_;

  // Thread pool members
  size_t _thread_pool_size;
  std::vector<std::thread> _worker_threads;
  std::queue<int> _connection_queue;
  std::mutex _queue_mutex;
  std::condition_variable _queue_condition;
  std::atomic<size_t> _active_threads;
  bool _shutdown;
};

}  // namespace cs::net::http

#endif  // CS_NET_HTTP_SERVER_HH