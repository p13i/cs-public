// cs/apps/trycopilot.ai/net/ws.cc
#include "cs/apps/trycopilot.ai/net/ws.hh"

namespace cs::apps::trycopilotai::net {

WsHub& GlobalWsHub() {
  static WsHub hub;
  return hub;
}

}  // namespace cs::apps::trycopilotai::net
