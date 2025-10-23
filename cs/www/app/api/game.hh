#ifndef CS_WWW_APP_API_GAME_HH
#define CS_WWW_APP_API_GAME_HH

#include "cs/net/proto/api.hh"
#include "cs/www/app/protos/game.proto.hh"

namespace cs::www::app::api {

DECLARE_API(GetAnimationAPI,
            cs::www::app::protos::GetAnimationRequest,
            cs::www::app::protos::GetAnimationResponse);

}  // namespace cs::www::app::api

#endif  // CS_WWW_APP_API_GAME_HH