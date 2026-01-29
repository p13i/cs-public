// cs/apps/trycopilot.ai/api/logs.hh
// cs/apps/trycopilot.ai/api/logs.hh
// cs/apps/trycopilot.ai/api/logs.hh
#ifndef CS_APPS_TRYCOPILOT_AI_API_LOGS_HH
#define CS_APPS_TRYCOPILOT_AI_API_LOGS_HH

#include "cs/apps/trycopilot.ai/protos/log.proto.hh"
#include "cs/net/proto/api.hh"

namespace cs::apps::trycopilotai::api {

DECLARE_API(
    CreateLogAPI,
    cs::apps::trycopilotai::protos::CreateLogRequest,
    cs::apps::trycopilotai::protos::CreateLogResponse);

DECLARE_API(GetLogAPI,
            cs::apps::trycopilotai::protos::GetLogRequest,
            cs::apps::trycopilotai::protos::GetLogResponse);

DECLARE_API(
    ListLogsAPI,
    cs::apps::trycopilotai::protos::ListLogsRequest,
    cs::apps::trycopilotai::protos::ListLogsResponse);

DECLARE_API(
    ListAppLogsAPI,
    cs::apps::trycopilotai::protos::ListAppLogsRequest,
    cs::apps::trycopilotai::protos::ListAppLogsResponse);

}  // namespace cs::apps::trycopilotai::api

#endif  // CS_APPS_TRYCOPILOT_AI_API_LOGS_HH