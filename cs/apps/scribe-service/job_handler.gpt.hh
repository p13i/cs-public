// cs/apps/scribe-service/job_handler.gpt.hh
#ifndef CS_APPS_SCRIBE_SERVICE_JOB_HANDLER_GPT_HH
#define CS_APPS_SCRIBE_SERVICE_JOB_HANDLER_GPT_HH

#include "cs/net/http/handler.hh"

namespace cs::apps::scribe_service {

struct ScribeContext;

DECLARE_HANDLER(PostJobLog, ScribeContext);
DECLARE_HANDLER(PostJobComplete, ScribeContext);

}  // namespace cs::apps::scribe_service

#endif  // CS_APPS_SCRIBE_SERVICE_JOB_HANDLER_GPT_HH
