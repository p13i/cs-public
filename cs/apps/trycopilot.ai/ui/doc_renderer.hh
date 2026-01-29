// cs/apps/trycopilot.ai/ui/doc_renderer.hh
#ifndef CS_APPS_TRYCOPILOT_AI_UI_DOC_RENDERER_HH
#define CS_APPS_TRYCOPILOT_AI_UI_DOC_RENDERER_HH

#include <ostream>

#include "cs/apps/trycopilot.ai/protos/ui.proto.hh"

namespace cs::apps::trycopilotai::ui {

inline void RenderDocToStream(
    const cs::apps::trycopilotai::protos::UiDoc& doc,
    std::ostream& os) {
  os << "Doc version: " << doc.version << "\n";
  if (!doc.message.empty()) {
    os << "Message: " << doc.message << "\n";
  }
  os << "Fields:\n";
  for (const auto& f : doc.fields) {
    os << "  " << f.name << ": " << f.value << "\n";
  }
}

}  // namespace cs::apps::trycopilotai::ui

#endif  // CS_APPS_TRYCOPILOT_AI_UI_DOC_RENDERER_HH
