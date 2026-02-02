// cs/net/rpc/rpc.hh
#ifndef CS_NET_RPC_RPC_HH
#define CS_NET_RPC_RPC_HH

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
#include "cs/util/di/context.gpt.hh"

// Requires four arguments. Use ::cs::util::di::Context<>
// (or CTX()) as ctx_ when the RPC does not use context.
#define DECLARE_RPC(rpc_, req_, res_, ctx_)               \
  class rpc_ : public ::cs::net::rpc::RPCBaseSelector<    \
                   rpc_, req_, res_, ctx_>::type {        \
   public:                                                \
    using Base =                                          \
        ::cs::net::rpc::RPCBaseSelector<rpc_, req_, res_, \
                                        ctx_>::type;      \
    using Base::Base;                                     \
    cs::ResultOr<res_> Process(                           \
        const req_& request) override;                    \
  };

#define IMPLEMENT_RPC(rpc_, req_, res_) \
  ::cs::ResultOr<res_> rpc_::Process(const req_& request)

namespace cs::net::rpc {

std::string QueryParamsToJson(
    std::map<std::string, std::string> map);

// Helper to match Content-Type against a base media type,
// ignoring parameters like charset.
inline bool MatchesContentType(
    const std::string& content_type,
    const std::string& expected) {
  std::string base = content_type;
  size_t semi = content_type.find(';');
  if (semi != std::string::npos) {
    base = content_type.substr(0, semi);
  }
  while (!base.empty() &&
         static_cast<unsigned char>(base.back()) <= ' ') {
    base.pop_back();
  }
  return base == expected;
}

template <typename T>
ResultOr<T> ExtractProtoFromRequest(
    cs::net::http::Request request) {
  std::string json_body = "{}";
  if (request.method() == "GET") {
    json_body = QueryParamsToJson(request._query_params);
  } else if (request.method() == "POST" ||
             request.method() == "PUT" ||
             request.method() == "PATCH" ||
             request.method() == "DELETE") {
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
    if (MatchesContentType(
            content_type,
            cs::net::http::
                kContentTypeXWwwFormUrlEncoded)) {
      std::map<std::string, std::string> form_data =
          request.ParseFormUrlEncoded(request.body());
      json_body = QueryParamsToJson(form_data);
    } else if (MatchesContentType(
                   content_type,
                   cs::net::http::
                       kContentTypeApplicationJson)) {
      json_body = request.body();
    }
  } else {
    return cs::InvalidArgument("Unknown HTTP method '" +
                               request.method() + "'");
  }
  return T().Parse(json_body);
};

// Consolidated RPC: 4 template params, use
// ::cs::util::di::Context<> when no context.
// HttpHandler(Request, AppCtx&) receives context
// as second parameter.
template <typename Implementation, typename RequestProto,
          typename ResponseProto, typename ContextType>
class RPC {
 public:
  using RequestType = RequestProto;
  using ResponseType = ResponseProto;

  template <typename AppCtx = ContextType>
  static cs::net::http::Response HttpHandler(
      cs::net::http::Request request, AppCtx& ctx) {
    RequestProto request_proto;
    SET_OR_RET(
        request_proto,
        ExtractProtoFromRequest<RequestProto>(request));
    Implementation implementation(ctx);
    SET_OR_RET(ResponseProto response,
               implementation.Process(request_proto));
    return cs::net::http::Response(
        cs::net::http::HTTP_200_OK,
        cs::net::http::kContentTypeApplicationJson,
        response.Serialize());
  }

  RPC() : ctx_(nullptr) {}

  template <typename Ctx>
  explicit RPC(Ctx& ctx) : ctx_(InitContext(ctx)) {
    if constexpr (!std::is_same_v<
                      ContextType,
                      ::cs::util::di::Context<>>) {
      static_assert(
          cs::util::di::SubsetOf<ContextType, Ctx>::value,
          "Context must provide all types required by this "
          "RPC");
    }
  }

  template <typename Ctx>
  static ContextType* InitContext(Ctx& ctx) {
    if constexpr (std::is_same_v<
                      ContextType,
                      ::cs::util::di::Context<>>) {
      (void)ctx;
      static ::cs::util::di::Context<> default_ctx;
      return &default_ctx;
    } else {
      return &ctx;
    }
  }

  virtual ~RPC() = default;

  virtual ResultOr<ResponseProto> Process(
      const RequestProto& request) = 0;

 protected:
  ResultOr<ContextType*> GetContext() const {
    if (ctx_ == nullptr) {
      return cs::InvalidArgument("Context not set");
    }
    return ctx_;
  }

 private:
  ContextType* ctx_;
};

template <typename Rpc, typename Req, typename Res,
          typename Ctx>
struct RPCBaseSelector {
  using type = RPC<Rpc, Req, Res, Ctx>;
};
}  // namespace cs::net::rpc

#endif  // CS_NET_RPC_RPC_HH