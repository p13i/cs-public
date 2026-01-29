// cs/apps/trycopilot.ai/protos/ui.proto.hh
#ifndef CS_APPS_TRYCOPILOT_AI_PROTOS_UI_PROTO_HH
#define CS_APPS_TRYCOPILOT_AI_PROTOS_UI_PROTO_HH

#include <string>

#include "cs/net/proto/proto.hh"

namespace cs::apps::trycopilotai::protos {

// UI event payload.
DECLARE_PROTO(UiEvent) {
  // Handler identifier.
  std::string handler_id;
  // Event payload.
  std::string payload;
};

// UI patch payload.
DECLARE_PROTO(UiPatch) {
  // Handler identifier.
  std::string handler_id;
  // Patch payload.
  std::string payload;
  // Patch version.
  int version;
  // Patch handled flag.
  bool handled;
  // Error message text.
  std::string error_message;
};

// Reusable UI patch envelope.
DECLARE_PROTO(UiPatchData) {
  // Handler identifier.
  std::string handler_id;
  // Patch payload.
  std::string payload;
  // Patch version.
  int version;
  // Patch message.
  std::string message;
};

// Code page request.
DECLARE_PROTO(GetCodePageRequest) {
  // Format identifier.
  std::string fmt;
};

// UI field entry.
DECLARE_PROTO(UiField) {
  // Field name.
  std::string name;
  // Field value.
  std::string value;
};

// UI form payload.
DECLARE_PROTO(UiFormPayload) {
  // Handler identifier.
  std::string handler_id;
  // Form fields.
  std::vector<UiField> fields;
};

// UI document field.
DECLARE_PROTO(UiDocField) {
  // Field name.
  std::string name;
  // Field value.
  std::string value;
};

// UI document payload.
DECLARE_PROTO(UiDoc) {
  // Document UUID.
  std::string uuid;
  // Document fields.
  std::vector<UiDocField> fields;
  // Document message.
  std::string message;
  // Document version.
  int version;
};

// UI doc request.
DECLARE_PROTO(UiDocRequest) {
  // Handler identifier.
  std::string handler_id;
};

// UI doc response.
DECLARE_PROTO(UiDocResponse) {
  // Document payload.
  UiDoc doc;
};

}  // namespace cs::apps::trycopilotai::protos

#endif  // CS_APPS_TRYCOPILOT_AI_PROTOS_UI_PROTO_HH
