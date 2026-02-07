// cs/apps/scribe-service/ui.gpt.hh
#ifndef CS_APPS_SCRIBE_SERVICE_UI_GPT_HH
#define CS_APPS_SCRIBE_SERVICE_UI_GPT_HH

#include "cs/net/http/handler.hh"

namespace cs::apps::scribe_service {

struct ScribeContext;

DECLARE_HANDLER(GetUploadPage, ScribeContext);
DECLARE_HANDLER(PostUpload, ScribeContext);
DECLARE_HANDLER(GetJobPage, ScribeContext);
DECLARE_HANDLER(GetJobTranscript, ScribeContext);

}  // namespace cs::apps::scribe_service

#endif  // CS_APPS_SCRIBE_SERVICE_UI_GPT_HH
