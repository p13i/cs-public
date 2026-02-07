// cs/apps/scribe-service/protos/upload.proto.hh
#ifndef CS_APPS_SCRIBE_SERVICE_PROTOS_UPLOAD_PROTO_HH
#define CS_APPS_SCRIBE_SERVICE_PROTOS_UPLOAD_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::scribe_service::protos {

// Form proto for the upload page (single file field
// "audio"); backend reads multipart, this is only used for
// HTML form generation.
DECLARE_PROTO(UploadAudioForm) { std::string audio; };

}  // namespace cs::apps::scribe_service::protos

#endif
