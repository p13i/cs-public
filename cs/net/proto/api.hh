#ifndef CS_NET_PROTO_API_HH
#define CS_NET_PROTO_API_HH

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

#include "cs/log.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/json/object.hh"
#include "cs/result.hh"

#define DECLARE_API(api_, req_, res_)                      \
  class api_ : public cs::net::proto::api::API<api_, req_, \
                                               res_> {     \
   public:                                                 \
    cs::ResultOr<res_> Process(                            \
        const req_& request) override;                     \
  }

#define IMPLEMENT_API(api_, req_, res_) \
  cs::ResultOr<res_> api_::Process(const req_& request)

namespace cs::net::proto::api {

std::string QueryParamsToJson(
    std::map<std::string, std::string> map);

template <typename T>
ResultOr<T> ExtractProtoFromRequest(
    cs::net::http::Request request) {
  std::string json_body = "{}";
  if (request.method() == "GET") {
    json_body = QueryParamsToJson(request._query_params);
  } else if (request.method() == "POST" ||
             request.method() == "PUT" ||
             request.method() == "PATCH") {
    // Determine content type, default to the HTML form.
    std::string content_type;
    if (request.headers().count("Content-Type") > 0) {
      content_type = request.headers()["Content-Type"];
    } else {
      LOG(WARNING)
          << "No Content-Type header found, "
          << "defaulting to "
          << cs::net::http::kContentTypeXWwwFormUrlEncoded
          << ENDL;
      content_type =
          cs::net::http::kContentTypeXWwwFormUrlEncoded;
    }

    // Use content type to parse user's input data.
    if (content_type ==
        cs::net::http::kContentTypeXWwwFormUrlEncoded) {
      std::map<std::string, std::string> form_data =
          request.ParseFormUrlEncoded(request.body());
      json_body = QueryParamsToJson(form_data);
    } else if (content_type ==
               cs::net::http::kContentTypeApplicationJson) {
      json_body = request.body();
    }
  } else {
    return cs::InvalidArgument("Unknown HTTP method '" +
                               request.method() + "'");
  }
  return T().Parse(json_body);
};

template <typename Implementation, typename RequestProto,
          typename ResponseProto>
class API {
 public:
  static cs::net::http::Response HttpHandler(
      cs::net::http::Request request) {
    RequestProto request_proto;
    SET_OR_RET(
        request_proto,
        ExtractProtoFromRequest<RequestProto>(request));
    Implementation implementation;
    SET_OR_RET(ResponseProto response,
               implementation.Process(request_proto));
    return cs::net::http::Response(
        cs::net::http::HTTP_200_OK,
        cs::net::http::kContentTypeApplicationJson,
        response.Serialize());
  }

  API() = default;
  virtual ~API() = default;

  // Override this method.
  virtual ResultOr<ResponseProto> Process(
      const RequestProto& request) = 0;
};
}  // namespace cs::net::proto::api

#endif  // CS_NET_PROTO_API_HH