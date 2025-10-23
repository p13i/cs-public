#ifndef CS_WWW_APP_API_USER_HH
#define CS_WWW_APP_API_USER_HH

#include "cs/net/proto/api.hh"
#include "cs/net/rpc/rpc.hh"
#include "cs/www/app/protos/user.proto.hh"

namespace cs::www::app::api {

DECLARE_API(CreateUserAPI,
            cs::www::app::protos::CreateUserRequest,
            cs::www::app::protos::CreateUserResponse);

}

namespace cs::www::app::rpc {

DECLARE_RPC(CreateUserRPC,
            cs::www::app::protos::CreateUserRequest,
            cs::www::app::protos::CreateUserResponse);

}  // namespace cs::www::app::rpc

#endif  // CS_WWW_APP_API_USER_HH