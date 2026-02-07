// cs/net/html/bootstrap/proto_form.gpt.hh
#ifndef CS_NET_HTML_BOOTSTRAP_PROTO_FORM_GPT_HH
#define CS_NET_HTML_BOOTSTRAP_PROTO_FORM_GPT_HH

#include <map>
#include <string>

#include "cs/net/html/dom.hh"
#include "cs/net/proto/form/proto_form.gpt.hh"

namespace cs::net::proto::form {

// Generate Bootstrap-styled HTML for a single supported
// field (div.mb-3, label.form-label, input.form-control or
// form-check-input).
template <typename T>
std::string GenerateBootstrapFormField(
    const cs::net::proto::protos::ProtoFieldMeta& field,
    const T* existing = nullptr) {
  if (!IsSupportedType(field.type)) {
    return "";
  }

  const auto config =
      GetInputConfig(field.type, field.name);

  std::map<std::string, std::string> attrs{
      {"id", field.name},
      {"name", field.name},
      {"type", config.type},
      {"required", ""},
  };

  if (config.step_any) {
    attrs["step"] = "any";
  }
  if (!config.accept.empty()) {
    attrs["accept"] = config.accept;
  }

  if (config.is_checkbox) {
    attrs.erase("type");
    attrs["class"] = "form-check-input";
  } else {
    attrs["class"] = "form-control";
  }

  std::string input_el =
      config.is_checkbox
          ? cs::net::html::dom::checkbox(attrs)
          : cs::net::html::dom::input(attrs);

  std::string label_class = config.is_checkbox
                                ? "form-check-label"
                                : "form-label";
  std::string label_el = cs::net::html::dom::label(
      {{"for", field.name}, {"class", label_class}},
      FormatLabel(field.name));

  std::string field_inner = config.is_checkbox
                                ? input_el + label_el
                                : label_el + input_el;
  std::string wrapper_class =
      config.is_checkbox ? "mb-3 form-check" : "mb-3";
  return cs::net::html::dom::div({{"class", wrapper_class}},
                                 field_inner);
}

// Generate a Bootstrap-styled form for proto T (form.mb-4,
// Bootstrap field layout, btn.btn-primary submit).
template <typename T>
std::string GenerateBootstrapProtoForm(
    const std::string& action,
    const std::string& method = "POST",
    const T* existing = nullptr,
    const std::string& enctype = "",
    const std::string& submit_label = "Submit") {
  (void)existing;

  const auto meta = T{}.Meta();

  std::map<std::string, std::string> form_attrs{
      {"action", action},
      {"method", method},
      {"class", "mb-4"},
  };
  if (!enctype.empty()) {
    form_attrs["enctype"] = enctype;
  }

  std::string body;
  for (const auto& field : meta.fields) {
    body += GenerateBootstrapFormField<T>(field, existing);
  }
  body += cs::net::html::dom::button(
      {{"type", "submit"}, {"class", "btn btn-primary"}},
      submit_label);

  return cs::net::html::dom::form(form_attrs, body);
}

}  // namespace cs::net::proto::form

#endif  // CS_NET_HTML_BOOTSTRAP_PROTO_FORM_GPT_HH
