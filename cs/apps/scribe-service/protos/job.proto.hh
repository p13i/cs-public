// cs/apps/scribe-service/protos/job.proto.hh
#ifndef CS_APPS_SCRIBE_SERVICE_PROTOS_JOB_PROTO_HH
#define CS_APPS_SCRIBE_SERVICE_PROTOS_JOB_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/proto/proto.hh"

namespace cs::apps::scribe_service::protos {

// Transcription job record (stored in database-service
// collection "scribe-jobs").
DECLARE_PROTO(TranscriptionJob) {
  [[required]] [[uuid]]
  std::string job_id;
  [[required]] [[enum_in("pending", "running", "completed",
                         "failed")]]
  std::string status;
  [[required]] [[uuid]]
  std::string blob_audio_uuid;
  std::vector<std::string> cli_output;
  std::string transcript_text;
};

}  // namespace cs::apps::scribe_service::protos

#endif  // CS_APPS_SCRIBE_SERVICE_PROTOS_JOB_PROTO_HH
