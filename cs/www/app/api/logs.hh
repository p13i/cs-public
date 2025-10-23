#ifndef CS_WWW_APP_API_LOGS_HH
#define CS_WWW_APP_API_LOGS_HH

#include "cs/net/proto/api.hh"
#include "cs/www/app/protos/log.proto.hh"

namespace cs::www::app::api {

DECLARE_API(CreateLogAPI,
            cs::www::app::protos::CreateLogRequest,
            cs::www::app::protos::CreateLogResponse);

DECLARE_API(GetLogAPI, cs::www::app::protos::GetLogRequest,
            cs::www::app::protos::GetLogResponse);

DECLARE_API(ListLogsAPI,
            cs::www::app::protos::ListLogsRequest,
            cs::www::app::protos::ListLogsResponse);

DECLARE_API(ListAppLogsAPI,
            cs::www::app::protos::ListAppLogsRequest,
            cs::www::app::protos::ListAppLogsResponse);

}  // namespace cs::www::app::api

#endif  // CS_WWW_APP_API_LOGS_HH