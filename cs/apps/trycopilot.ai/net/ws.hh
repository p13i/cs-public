// cs/apps/trycopilot.ai/net/ws.hh
#ifndef CS_APPS_TRYCOPILOT_AI_NET_WS_HH
#define CS_APPS_TRYCOPILOT_AI_NET_WS_HH

#include <functional>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"

namespace cs::apps::trycopilotai::net {

// Extremely small stub to broadcast text frames to
// connected websocket clients. This uses a trivial
// long-poll fallback since a full websocket server isn't
// present in the current codebase.
class WsHub {
 public:
  using ClientId = std::string;

  // Registers a client (using a unique id) and returns any
  // queued messages as newline-delimited text.
  std::string Register(const ClientId& id) {
    std::lock_guard<std::mutex> l(mu_);
    clients_.insert(id);
    return DrainQueueLocked();
  }

  void Unregister(const ClientId& id) {
    std::lock_guard<std::mutex> l(mu_);
    clients_.erase(id);
  }

  void Broadcast(const std::string& msg) {
    std::lock_guard<std::mutex> l(mu_);
    queue_.push_back(msg);
  }

  // Returns and clears the current queue.
  std::string DrainQueue() {
    std::lock_guard<std::mutex> l(mu_);
    return DrainQueueLocked();
  }

 private:
  std::string DrainQueueLocked() {
    std::string out;
    for (const auto& m : queue_) {
      out.append(m);
      out.push_back('\n');
    }
    queue_.clear();
    return out;
  }

  std::mutex mu_;
  std::unordered_set<ClientId> clients_;
  std::vector<std::string> queue_;
};

WsHub& GlobalWsHub();

}  // namespace cs::apps::trycopilotai::net

#endif  // CS_APPS_TRYCOPILOT_AI_NET_WS_HH
