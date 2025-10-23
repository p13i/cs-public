# Remote-procedure calls with JSON over HTTP

I would like to define APIs in a micro/subset language of
C++:

```cc
// api/item.hh
DECLARE_RPC(GET, GetItem,
  cs::www::app::protos::GetItemRequest,
  cs::www::app::protos::GetItemResponse);

// expands to:

class GetItemRPC : public RPC<GetItemRequest, GetItemResponse> {
   virtual cs::www::app::protos::GetItemResponse GET(const cs::www::app::protos::GetItemRequest request) = 0;
}
```

```cc
// api/item.cc
namespace {  // use_usings
using ::cs::www::app::protos::GetItemRequest;
using ::cs::www::app::protos::GetItemResponse;
}  // namespace

IMPLEMENT_RPC(GET, GetItem, GetItemResponse,
   [](const GetItemRequest& request) {
      // My c++ code here
      return {};
   });

// expands to:

class GetItemRPC : public RPC<GetItemRequest, GetItemResponse> {
public:
GetItemResponse GET(const GetItemRequest& request) override {
   return ([&](const GetItemRequest& request) {
      // My c++ code here
      return {};
   })(request);
}
};

// With this in cs/net/proto/rpc.hh:
template <typename RequestT, typename ResponseT>
class RPC<RequestT, ResponseT> {
virtual ResultOr<ResponseT> GET(const RequestT& request) {
   return NotImplemented();
}
virtual ResultOr<ResponseT> POST(const RequestT& request) {
   return NotImplemented();
}
virtual ResultOr<ResponseT> PATCH(const RequestT& request) {
   return NotImplemented();
}
virtual ResultOr<ResponseT> DELETE(const RequestT& request) {
   return NotImplemented();
}
ResultOr<RequestT> ParseProtoFromHttpRequest(const cs::net::http::Request& request) {
   // details hidden
}
};
```

```cc
#include "cs/net/rpc/service.hh"

// api/services.hh
DECLARE_SERVICE(Items, {
  GetItemRPC,
  // ...
});

// expands to:

class ItemsService : public Service {
   std::vector<RPC> GetRPCs() {
      return {
         GetItemRPC(),
         // ...
      };
   };
};

// With this in cs/net/rpc/service.hh
class Service {
   virtual std::vector<RPC> GetRPCs() = 0;
   cs::net::http::Response HttpHandler(const http::Request& request) {
      for (const auto& rpc : GetRPCs()) {
         if (request.method() == "GET") {
            return rpc.GET(request);
         } else if (request.method() == "POST") {
            return rpc.POST(request);
         } else if (request.method() == "PATCH") {
            return rpc.PATCH(request);
         } else if (request.method() == "DELETE") {
            return rpc.DELETE(request);
         }
      }
      return {};
   }
};
```

```cc
// app/main.cc
// ...
#include api/services.hh
// ...
int main() {
   WebApp app;
   REGISTER_RPC("/api/item/", ItemsService);
   // expands to:
   app.AddHandler("/api/item/", ItemsService::HttpHandler);
   app.Run(/*port=*/8080)
}
// ...
```

Now this API should be responsive to
`GET http://localhost:8080/api/item`.

This micro implementation of remote-produce calls (RPCs)
should automatically generate:

1. The "binding code" between this single declaration and
   the service being available on the port.

1. A class `ItemsService` that implements a
   `GetItemResponse GetItem(GetItemRequest request);` method
   that the application logic can implement.

1. Support for authentication and authorization
   restrictions.

1. Define a network of services and a service registry for
   clients to find an active host and port.
