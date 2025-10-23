#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "cs/fs/fs.hh"
#include "cs/log.hh"
#include "cs/net/http/favicon.hh"
#include "cs/net/http/request.hh"
#include "cs/net/http/response.hh"
#include "cs/net/http/server.hh"
#include "cs/net/http/web_app.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/json/serialize.hh"
#include "cs/net/proto/api.hh"
#include "cs/net/proto/db/db.hh"
#include "cs/result.hh"
#include "cs/util/timeit.hh"
#include "cs/util/uuid.hh"

namespace {  // use_usings
using ::cs::Error;
using ::cs::InvalidArgument;
using ::cs::NotFoundError;
using ::cs::Ok;
using ::cs::Result;
using ::cs::ValidationError;
using ::cs::net::http::Http301MovedPermanently;
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_201_CREATED;
using ::cs::net::http::HTTP_302_FOUND;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_403_PERMISSION_DENIED;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::kContentTypeApplicationJson;
using ::cs::net::http::kContentTypeTextHtml;
using ::cs::net::http::kContentTypeTextPlain;
using ::cs::net::http::Request;
using ::cs::net::http::Response;
using ::cs::net::http::WebApp;
using ::cs::net::json::Object;
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::proto::db::JsonProtoDatabase;
using ::cs::net::proto::db::LocalJsonDatabase;
using ::cs::parsers::ConsumePrefix;
using ::cs::parsers::ParseInt;
using ::cs::parsers::ParseUnsignedInt;
}  // namespace

Response GetSplashpage(Request request) { return Ok(); }

Result RunMyWebApp() {
  cs::net::http::WebApp app;

  ////////////////////////////////
  // SPECIAL APPS ////////////////
  ////////////////////////////////
  app.Register("GET", "/", GetSplashpage);

  return app.RunServer("0.0.0.0", 8091);
}

int main(int argc, char **argv) {
  if (argc != 1) {
    std::cerr << "Usage: " << argv[0] << "\n";
    std::cerr << "argv=[";
    for (int i = 0; i < argc; ++i) {
      std::cerr << argv[i] << " ";
    }
    std::cerr << "]\n" << ENDL;
    return 1;
  }

  Result result = RunMyWebApp();
  if (!result.ok()) {
    LOG(ERROR) << "Error running web app: "
               << result.message() << ENDL;
    std::cerr << result << ENDL;
  }
  return result.code();
}
