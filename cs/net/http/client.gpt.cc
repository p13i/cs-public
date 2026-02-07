// cs/net/http/client.gpt.cc
#include "cs/net/http/client.hh"

#include <curl/curl.h>

#include <map>
#include <sstream>
#include <string>

namespace {  // use_usings
using ::cs::net::http::HTTP_200_OK;
using ::cs::net::http::HTTP_201_CREATED;
using ::cs::net::http::HTTP_301_MOVED_PERMANENTLY;
using ::cs::net::http::HTTP_302_FOUND;
using ::cs::net::http::HTTP_400_BAD_REQUEST;
using ::cs::net::http::HTTP_403_PERMISSION_DENIED;
using ::cs::net::http::HTTP_404_NOT_FOUND;
using ::cs::net::http::HTTP_500_INTERNAL_SERVER_ERROR;
using ::cs::net::http::Response;
using ::cs::net::http::Status;
}  // namespace

namespace {  // helpers

static size_t CurlWriteCallback(void* contents, size_t size,
                                size_t nmemb, void* userp) {
  reinterpret_cast<std::string*>(userp)->append(
      (char*)contents, size * nmemb);
  return size * nmemb;
}

struct CurlResponseContext {
  std::string body;
  std::map<std::string, std::string> headers;
  long status_code = 0;
  std::string status_text;
};

static size_t CurlHeaderCallback(char* buffer, size_t size,
                                 size_t nmemb,
                                 void* userp) {
  auto* ctx = reinterpret_cast<CurlResponseContext*>(userp);
  size_t total = size * nmemb;
  std::string header_line(buffer, total);
  while (!header_line.empty() &&
         (header_line.back() == '\r' ||
          header_line.back() == '\n')) {
    header_line.pop_back();
  }
  if (header_line.rfind("HTTP/", 0) == 0) {
    std::istringstream iss(header_line);
    std::string http_version;
    iss >> http_version >> ctx->status_code;
    std::getline(iss, ctx->status_text);
    while (!ctx->status_text.empty() &&
           ctx->status_text.front() == ' ') {
      ctx->status_text.erase(ctx->status_text.begin());
    }
    return total;
  }
  auto colon = header_line.find(':');
  if (colon != std::string::npos) {
    std::string name = header_line.substr(0, colon);
    std::string value =
        header_line.substr(colon + 1, std::string::npos);
    while (!value.empty() && value.front() == ' ') {
      value.erase(value.begin());
    }
    ctx->headers[name] = value;
  }
  return total;
}

static Status ToStatus(long code, const std::string& name) {
  switch (code) {
    case 200:
      return HTTP_200_OK;
    case 201:
      return HTTP_201_CREATED;
    case 301:
      return HTTP_301_MOVED_PERMANENTLY;
    case 302:
      return HTTP_302_FOUND;
    case 400:
      return HTTP_400_BAD_REQUEST;
    case 403:
      return HTTP_403_PERMISSION_DENIED;
    case 404:
      return HTTP_404_NOT_FOUND;
    case 500:
      return HTTP_500_INTERNAL_SERVER_ERROR;
    default:
      return Status(static_cast<uint32_t>(code),
                    name.size() > 0 ? name : "UNKNOWN");
  }
}

}  // namespace

namespace cs {

ResultOr<std::string> Fetch(
    const std::string& url, const std::string& method,
    const std::map<std::string, std::string>& headers,
    const std::string& body) {
  LOG(DEBUG) << "Fetching URL: " << url
             << " with method: " << method << ENDL;
  CURL* curl = curl_easy_init();
  if (!curl) return Error("Failed to initialize curl");

  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   CurlWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  if (method != "GET") {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,
                     method.c_str());
    if (!body.empty()) {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                       body.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                       body.size());
    }
  }

  // User-Agent: CoolBot/0.0 (https://example.org/coolbot/;
  // coolbot@example.org) generic-library/0.0
  curl_easy_setopt(curl, CURLOPT_USERAGENT,
                   "GptBot/0.0 (https://cs.p13i.io; "
                   "no-reply@p13i.io) generic-library/0.0");
  // Accept-Encoding: gzip
  //   curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,
  //   "gzip");

  // Set timeout to 30 seconds
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

  struct curl_slist* header_list = nullptr;
  for (const auto& [k, v] : headers) {
    header_list = curl_slist_append(header_list,
                                    (k + ": " + v).c_str());
  }
  if (header_list)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::string err = curl_easy_strerror(res);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    LOG(DEBUG) << "Fetch response error (hostname=" << url
               << ") : " << err << ENDL;
    return Error(err);
  }

  LOG(DEBUG) << "Fetch response body: "
             << response.substr(0, 64) << "..." << ENDL;

  curl_slist_free_all(header_list);
  curl_easy_cleanup(curl);
  return response;
}

ResultOr<Response> FetchResponse(
    const std::string& url, const std::string& method,
    const std::map<std::string, std::string>& headers,
    const std::string& body) {
  LOG(DEBUG) << "Fetching URL: " << url
             << " with method: " << method << ENDL;

  CURL* curl = curl_easy_init();
  if (!curl) return Error("Failed to initialize curl");

  CurlResponseContext context;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   CurlWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &context.body);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,
                   CurlHeaderCallback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &context);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);

  if (method == "GET") {
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
  } else {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,
                     method.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                     body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                     body.size());
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT,
                   "GptBot/0.0 (https://cs.p13i.io; "
                   "no-reply@p13i.io) generic-library/0.0");

  // Set timeout to 30 seconds
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

  struct curl_slist* header_list = nullptr;
  for (const auto& [k, v] : headers) {
    header_list = curl_slist_append(header_list,
                                    (k + ": " + v).c_str());
  }
  if (header_list)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

  CURLcode res = curl_easy_perform(curl);
  long code_from_info = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                    &code_from_info);

  if (res != CURLE_OK) {
    std::string err = curl_easy_strerror(res);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    return Error(err);
  }

  LOG(DEBUG) << "FetchResponse body: "
             << context.body.substr(0, 64) << "..." << ENDL;

  curl_slist_free_all(header_list);
  curl_easy_cleanup(curl);

  if (context.status_code == 0 && code_from_info != 0) {
    context.status_code = code_from_info;
  }
  if (context.status_code == 0) {
    context.status_code = 500;
    context.status_text = "INTERNAL SERVER ERROR";
  }

  Status status =
      ToStatus(context.status_code, context.status_text);
  std::string content_type =
      cs::net::http::kContentTypeTextPlain;
  auto found = context.headers.find("Content-Type");
  if (found != context.headers.end()) {
    content_type = found->second;
    context.headers.erase(found);
  }

  Response response(status, content_type, context.body);
  response._headers = context.headers;
  return response;
}

}  // namespace cs
