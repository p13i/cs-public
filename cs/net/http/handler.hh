// cs/net/http/handler.hh
#ifndef CS_NET_HTTP_HANDLER_HH
#define CS_NET_HTTP_HANDLER_HH

#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"

// Declares an HTTP handler: Response _name(Request request,
// _ctx& ctx). Use like DECLARE_HANDLER(GetUploadPage,
// ScribeContext) or DECLARE_HANDLER(GetIndexPage,
// UIContext).
#define DECLARE_HANDLER(_name, _ctx) \
  ::cs::net::http::Response _name(   \
      ::cs::net::http::Request request, _ctx& ctx)

#endif  // CS_NET_HTTP_HANDLER_HH
