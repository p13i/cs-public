#include <curl/curl.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Global state for recursion:
static std::unordered_map<std::string, std::string>
    visited;  // Maps URL -> local filename
static std::string outputFolder;
static std::string
    baseDomain;  // Domain of the main URL (for restricting
                 // recursion)
static bool inlineJS =
    true;  // Control for inlining JS vs saving to file

// Curl write callback to accumulate response data into a
// buffer
size_t writeDataCallback(void* ptr, size_t size,
                         size_t nmemb, void* userdata) {
  std::vector<char>* recvBuffer =
      static_cast<std::vector<char>*>(userdata);
  size_t total = size * nmemb;
  if (total > 0) {
    recvBuffer->insert(recvBuffer->end(), (char*)ptr,
                       (char*)ptr + total);
  }
  return total;
}

// Fetch the URL content into a byte buffer, and optionally
// get the Content-Type
bool fetchUrl(const std::string& url,
              std::vector<char>& data,
              std::string& outContentType) {
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error: curl init failed\n";
    return false;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   writeDataCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(
      curl, CURLOPT_ACCEPT_ENCODING,
      "");  // allow libcurl to handle gzip/deflate
  curl_easy_setopt(curl, CURLOPT_USERAGENT,
                   "SingleFile/1.0");
  // (SSL verification left default true; could be adjusted
  // if needed)
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Error: Failed to fetch " << url << " - "
              << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(curl);
    return false;
  }
  char* ctype = nullptr;
  curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
  if (ctype && outContentType.empty()) {
    outContentType = std::string(ctype);
  }
  curl_easy_cleanup(curl);
  return true;
}

// Base64-encode binary data (for embedding images, fonts,
// etc.)
std::string base64Encode(const unsigned char* data,
                         size_t len) {
  static const char* base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      "0123456789+/";
  std::string result;
  result.reserve(((len + 2) / 3) * 4);
  unsigned int val = 0;
  int valb = -6;
  for (size_t i = 0; i < len; ++i) {
    val = (val << 8) + (unsigned char)data[i];
    valb += 8;
    while (valb >= 0) {
      result.push_back(base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    result.push_back(
        base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  while (result.size() % 4) {
    result.push_back('=');
  }
  return result;
}
inline std::string base64Encode(
    const std::vector<char>& data) {
  return base64Encode(
      reinterpret_cast<const unsigned char*>(data.data()),
      data.size());
}

// Sanitize a URL into a safe filename (replacing unsafe
// chars with '-' etc.)
std::string sanitizeUrlToFilename(
    const std::string& url,
    const std::string& ext = ".html") {
  // Remove scheme (http://, https://, etc.)
  std::string s = url;
  size_t schemePos = s.find("://");
  if (schemePos != std::string::npos) {
    s = s.substr(schemePos + 3);
  }
  // Strip trailing slashes
  while (!s.empty() &&
         (s.back() == '/' || s.back() == '\\')) {
    s.pop_back();
  }
  // Replace unsafe characters with '-'
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') || c == '.' || c == '_' ||
        c == '-') {
      out.push_back(c);
    } else {
      out.push_back('-');
    }
  }
  // Collapse consecutive '-'
  std::string collapsed;
  collapsed.reserve(out.size());
  bool lastDash = false;
  for (char c : out) {
    if (c == '-') {
      if (lastDash) continue;
      lastDash = true;
      collapsed.push_back('-');
    } else {
      lastDash = false;
      collapsed.push_back(c);
    }
  }
  // Trim leading/trailing '-'
  if (!collapsed.empty() && collapsed.front() == '-') {
    size_t i = 0;
    while (i < collapsed.size() && collapsed[i] == '-') i++;
    collapsed.erase(0, i);
  }
  if (!collapsed.empty() && collapsed.back() == '-') {
    size_t i = collapsed.size();
    while (i > 0 && collapsed[i - 1] == '-') i--;
    collapsed.erase(i);
  }
  // Cap length (max 180 chars to avoid extremely long
  // filenames)
  if (collapsed.size() > 180) {
    collapsed.resize(180);
    if (!collapsed.empty() && collapsed.back() == '-') {
      size_t i = collapsed.size();
      while (i > 0 && collapsed[i - 1] == '-') i--;
      collapsed.erase(i);
    }
  }
  if (collapsed.empty()) {
    collapsed = "file";
  }
  // Append extension if not already present
  if (!ext.empty()) {
    if (collapsed.size() < ext.size() ||
        collapsed.substr(collapsed.size() - ext.size()) !=
            ext) {
      collapsed += ext;
    }
  }
  return collapsed;
}

// Resolve a relative URL against a base URL
std::string resolveUrl(const std::string& baseUrl,
                       const std::string& relative) {
  if (relative.empty()) return baseUrl;
  // If already absolute or a data/mailto link, return as is
  if (relative.find("://") != std::string::npos ||
      relative.rfind("data:", 0) == 0 ||
      relative.rfind("mailto:", 0) == 0 ||
      relative.rfind("javascript:", 0) == 0) {
    return relative;
  }
  // If protocol-relative (starts with "//"), prepend base
  // scheme
  if (relative.rfind("//", 0) == 0) {
    size_t schemePos = baseUrl.find("://");
    if (schemePos != std::string::npos) {
      std::string scheme = baseUrl.substr(0, schemePos);
      return scheme + ":" + relative;
    }
    return "http:" + relative;
  }
  // Break baseUrl into components (scheme, host, path)
  size_t schemePos = baseUrl.find("://");
  std::string scheme, host, path;
  if (schemePos != std::string::npos) {
    scheme = baseUrl.substr(0, schemePos);
    size_t hostPos = schemePos + 3;
    size_t pathPos = baseUrl.find('/', hostPos);
    if (pathPos == std::string::npos) {
      host = baseUrl.substr(hostPos);
      path = "/";
    } else {
      host = baseUrl.substr(hostPos, pathPos - hostPos);
      path = baseUrl.substr(pathPos);
    }
  } else {
    // If baseUrl has no scheme (unlikely here), treat the
    // whole as host.
    host = baseUrl;
    path = "/";
  }
  if (relative[0] == '/') {
    // Root-relative path
    return scheme + "://" + host + relative;
  }
  // Otherwise, relative to base path
  std::string baseDir = path;
  if (baseDir.empty()) baseDir = "/";
  if (baseDir.back() != '/') {
    size_t slashPos = baseDir.find_last_of('/');
    if (slashPos != std::string::npos) {
      baseDir = baseDir.substr(0, slashPos + 1);
    } else {
      baseDir = "/";
    }
  }
  std::string mergedPath = baseDir + relative;
  // Normalize "./" and "../" segments in the path
  std::vector<std::string> segments;
  size_t start = 0;
  while (start < mergedPath.size()) {
    if (mergedPath[start] == '/') {
      start++;
      continue;
    }
    size_t end = mergedPath.find('/', start);
    std::string seg =
        (end == std::string::npos)
            ? mergedPath.substr(start)
            : mergedPath.substr(start, end - start);
    start = (end == std::string::npos) ? mergedPath.size()
                                       : end + 1;
    if (seg == "." || seg.empty()) {
      continue;
    } else if (seg == "..") {
      if (!segments.empty()) segments.pop_back();
    } else {
      segments.push_back(seg);
    }
  }
  std::string normPath = "/";
  for (size_t i = 0; i < segments.size(); ++i) {
    normPath += segments[i];
    if (i < segments.size() - 1) normPath += "/";
  }
  // Preserve trailing slash if present in mergedPath
  if (!mergedPath.empty() && mergedPath.back() == '/' &&
      normPath.back() != '/') {
    normPath += "/";
  }
  return scheme + "://" + host + normPath;
}

// Process CSS content: inline @imports and replace url(...)
// with data URIs
std::string processCss(const std::string& cssText,
                       const std::string& cssBaseUrl) {
  std::string css = cssText;
  // Handle @import directives (recursively inline imported
  // CSS)
  size_t importPos = 0;
  while ((importPos = css.find("@import", importPos)) !=
         std::string::npos) {
    size_t startPos = importPos;
    size_t endPos = std::string::npos;
    std::string importUrl;
    // Two possible syntaxes: @import "file.css" or @import
    // url("file.css")
    size_t urlPos = css.find("url(", importPos);
    size_t quotePos = css.find('"', importPos);
    if (urlPos != std::string::npos &&
        urlPos < css.find(';', importPos)) {
      // @import url("...")
      size_t openParen = css.find('(', urlPos);
      size_t firstQuote =
          css.find_first_of("\"'", openParen);
      size_t secondQuote =
          (firstQuote != std::string::npos)
              ? css.find(css[firstQuote], firstQuote + 1)
              : std::string::npos;
      if (firstQuote != std::string::npos &&
          secondQuote != std::string::npos) {
        importUrl = css.substr(
            firstQuote + 1, secondQuote - firstQuote - 1);
        endPos = css.find(';', importPos);
        if (endPos != std::string::npos)
          endPos++;  // include ';'
      } else {
        importPos +=
            7;  // skip this occurrence and continue
        continue;
      }
    } else if (quotePos != std::string::npos &&
               quotePos < css.find(';', importPos)) {
      // @import "..."
      size_t secondQuote = css.find('"', quotePos + 1);
      if (secondQuote != std::string::npos) {
        importUrl = css.substr(quotePos + 1,
                               secondQuote - quotePos - 1);
        endPos = css.find(';', importPos);
        if (endPos != std::string::npos) endPos++;
      } else {
        importPos += 7;
        continue;
      }
    } else {
      importPos += 7;
      continue;
    }
    importUrl = resolveUrl(cssBaseUrl, importUrl);
    // Fetch and inline the imported CSS
    std::vector<char> cssData;
    std::string cssType;
    if (fetchUrl(importUrl, cssData, cssType)) {
      std::string importContent(cssData.begin(),
                                cssData.end());
      std::string inlinedCSS = processCss(
          importContent, importUrl);  // recursive inline
      // Preserve media queries: if @import had media
      // conditions (e.g., @import url(...){media})
      std::string mediaRule;
      size_t mediaStart = css.find(importUrl, startPos);
      if (mediaStart != std::string::npos) {
        mediaStart += importUrl.size();
        // Skip any closing quotes/parens and whitespace
        while (mediaStart < css.size() &&
               (css[mediaStart] == ')' ||
                css[mediaStart] == '"' ||
                css[mediaStart] == '\'' ||
                isspace(css[mediaStart]))) {
          mediaStart++;
        }
        if (endPos != std::string::npos &&
            mediaStart < endPos - 1) {
          mediaRule = css.substr(mediaStart,
                                 endPos - mediaStart - 1);
        }
      }
      std::string replacement;
      if (!mediaRule.empty()) {
        replacement = "@media " + mediaRule + " {\n" +
                      inlinedCSS + "\n}\n";
      } else {
        replacement = inlinedCSS;
      }
      css.replace(startPos,
                  (endPos == std::string::npos
                       ? css.size() - startPos
                       : endPos - startPos),
                  replacement);
      importPos = startPos +
                  replacement.size();  // advance position
    } else {
      // If import fetch fails, remove the @import rule
      css.erase(startPos, (endPos == std::string::npos
                               ? css.size() - startPos
                               : endPos - startPos));
    }
  }
  // Replace url(...) references (images, fonts) with base64
  // data URIs
  size_t pos = 0;
  while ((pos = css.find("url(", pos)) !=
         std::string::npos) {
    size_t openParen = css.find('(', pos);
    if (openParen == std::string::npos) break;
    size_t closeParen = css.find(')', openParen);
    if (closeParen == std::string::npos) break;
    std::string inner = css.substr(
        openParen + 1, closeParen - openParen - 1);
    // Trim quotes and whitespace inside url()
    inner.erase(0, inner.find_first_not_of(" \t\n\r'\""));
    inner.erase(inner.find_last_not_of(" \t\n\r'\"") + 1);
    if (inner.empty() || inner.rfind("data:", 0) == 0) {
      pos = closeParen + 1;
      continue;  // already a data URI or empty
    }
    std::string resUrl = resolveUrl(cssBaseUrl, inner);
    std::vector<char> resourceData;
    std::string resType;
    if (fetchUrl(resUrl, resourceData, resType)) {
      std::string mimeType;
      if (!resType.empty()) {
        // Use Content-Type from HTTP if available (strip
        // charset)
        size_t sc = resType.find(';');
        mimeType =
            (sc != std::string::npos ? resType.substr(0, sc)
                                     : resType);
      }
      if (mimeType.empty()) {
        // Guess MIME from URL file extension
        if (resUrl.find(".png") != std::string::npos)
          mimeType = "image/png";
        else if (resUrl.find(".jpg") != std::string::npos ||
                 resUrl.find(".jpeg") != std::string::npos)
          mimeType = "image/jpeg";
        else if (resUrl.find(".gif") != std::string::npos)
          mimeType = "image/gif";
        else if (resUrl.find(".svg") != std::string::npos)
          mimeType = "image/svg+xml";
        else if (resUrl.find(".webp") != std::string::npos)
          mimeType = "image/webp";
        else if (resUrl.find(".woff2") != std::string::npos)
          mimeType = "font/woff2";
        else if (resUrl.find(".woff") != std::string::npos)
          mimeType = "font/woff";
        else if (resUrl.find(".ttf") != std::string::npos)
          mimeType = "font/ttf";
        else
          mimeType = "application/octet-stream";
      }
      std::string b64 = base64Encode(
          reinterpret_cast<const unsigned char*>(
              resourceData.data()),
          resourceData.size());
      std::string dataUri = "url(\"data:" + mimeType +
                            ";base64," + b64 + "\")";
      css.replace(pos, closeParen - pos + 1, dataUri);
      pos += dataUri.size();
    } else {
      // If resource fetch fails, leave the url() reference
      // as-is (it may just not load offline)
      pos = closeParen + 1;
    }
  }
  return css;
}

// Core function: process a single page (and possibly
// recursive links) into a self-contained HTML string
std::string SingleFile(const std::string& pageUrl,
                       unsigned int depth) {
  // Fetch main HTML content
  std::vector<char> rawData;
  std::string contentType;
  if (!fetchUrl(pageUrl, rawData, contentType)) {
    return std::string();  // return empty string on fetch
                           // failure
  }
  std::string html(rawData.begin(), rawData.end());
  // Determine base URL for resolving relative references
  // (consider <base href> if present)
  std::string pageBaseUrl = pageUrl;
  size_t basePos = html.find("<base");
  if (basePos != std::string::npos) {
    size_t hrefPos = html.find("href", basePos);
    if (hrefPos != std::string::npos) {
      size_t eqPos = html.find('=', hrefPos);
      size_t quotePos = html.find_first_of("\"'", eqPos);
      if (eqPos != std::string::npos &&
          quotePos != std::string::npos) {
        char qch = html[quotePos];
        size_t endQuote = html.find(qch, quotePos + 1);
        if (endQuote != std::string::npos) {
          std::string baseHref = html.substr(
              quotePos + 1, endQuote - quotePos - 1);
          if (!baseHref.empty()) {
            pageBaseUrl = resolveUrl(pageUrl, baseHref);
          }
        }
      }
    }
    // Remove the base tag from the HTML to avoid
    // interfering with local links
    size_t baseEnd = html.find('>', basePos);
    if (baseEnd != std::string::npos) {
      html.erase(basePos, baseEnd - basePos + 1);
    }
  }
  std::string result;
  result.reserve(html.size());
  // Iterate through the HTML, handling specific tags
  for (size_t i = 0; i < html.size();) {
    if (html[i] == '<') {
      // Check for known tag openings (case-insensitive)
      if ((html.compare(i, 5, "<link") == 0) ||
          (html.compare(i, 5, "<LINK") == 0)) {
        size_t tagEnd = html.find('>', i);
        if (tagEnd == std::string::npos) break;
        std::string tag = html.substr(i, tagEnd - i + 1);
        // Identify rel attribute
        std::string relVal;
        size_t relPos = tag.find("rel");
        if (relPos != std::string::npos) {
          size_t relEq = tag.find('=', relPos);
          size_t relValStart =
              tag.find_first_of("\"'", relEq);
          if (relEq != std::string::npos &&
              relValStart != std::string::npos) {
            char q = tag[relValStart];
            size_t relValEnd = tag.find(q, relValStart + 1);
            if (relValEnd != std::string::npos) {
              relVal =
                  tag.substr(relValStart + 1,
                             relValEnd - relValStart - 1);
            }
          }
        }
        std::string relLower = relVal;
        std::transform(relLower.begin(), relLower.end(),
                       relLower.begin(), ::tolower);
        if (relLower == "stylesheet") {
          // Inline external CSS
          size_t hrefPos = tag.find("href");
          std::string cssUrl;
          if (hrefPos != std::string::npos) {
            size_t eq = tag.find('=', hrefPos);
            size_t q = tag.find_first_of("\"'", eq);
            if (eq != std::string::npos &&
                q != std::string::npos) {
              char qch = tag[q];
              size_t endq = tag.find(qch, q + 1);
              if (endq != std::string::npos) {
                cssUrl = tag.substr(q + 1, endq - q - 1);
              }
            }
          }
          if (!cssUrl.empty()) {
            cssUrl = resolveUrl(pageBaseUrl, cssUrl);
            std::vector<char> cssData;
            std::string cssType;
            if (fetchUrl(cssUrl, cssData, cssType)) {
              std::string cssText(cssData.begin(),
                                  cssData.end());
              std::string inlinedCss =
                  processCss(cssText, cssUrl);
              // Preserve media attribute if present (e.g.,
              // media="print")
              std::string mediaAttr;
              size_t mediaPos = tag.find("media");
              if (mediaPos != std::string::npos) {
                size_t eq = tag.find('=', mediaPos);
                size_t q = tag.find_first_of("\"'", eq);
                if (eq != std::string::npos &&
                    q != std::string::npos) {
                  char qch = tag[q];
                  size_t endq = tag.find(qch, q + 1);
                  if (endq != std::string::npos) {
                    mediaAttr =
                        tag.substr(q + 1, endq - q - 1);
                  }
                }
              }
              // Replace <link> with <style> containing the
              // inlined CSS
              result += "<style";
              if (!mediaAttr.empty()) {
                result += " media=\"" + mediaAttr + "\"";
              }
              result += ">" + inlinedCss + "</style>";
            } else {
              // If CSS fetch fails, keep the original link
              // tag as a fallback
              result += tag;
            }
          } else {
            result += tag;  // no href (shouldn't happen for
                            // stylesheet), just copy
          }
          i = tagEnd + 1;
          continue;
        } else if (relLower.find("icon") !=
                   std::string::npos) {
          // Inline favicon or apple-touch-icon
          size_t hrefPos = tag.find("href");
          std::string iconUrl;
          if (hrefPos != std::string::npos) {
            size_t eq = tag.find('=', hrefPos);
            size_t q = tag.find_first_of("\"'", eq);
            if (eq != std::string::npos &&
                q != std::string::npos) {
              char qch = tag[q];
              size_t endq = tag.find(qch, q + 1);
              if (endq != std::string::npos) {
                iconUrl = tag.substr(q + 1, endq - q - 1);
              }
            }
          }
          if (!iconUrl.empty()) {
            iconUrl = resolveUrl(pageBaseUrl, iconUrl);
            std::vector<char> iconData;
            std::string iconType;
            if (fetchUrl(iconUrl, iconData, iconType)) {
              // Determine icon MIME type (from content type
              // or file extension)
              std::string mimeType;
              if (!iconType.empty()) {
                size_t sc = iconType.find(';');
                mimeType = (sc != std::string::npos
                                ? iconType.substr(0, sc)
                                : iconType);
              }
              if (mimeType.empty()) {
                if (iconUrl.find(".png") !=
                    std::string::npos)
                  mimeType = "image/png";
                else if (iconUrl.find(".jpg") !=
                             std::string::npos ||
                         iconUrl.find(".jpeg") !=
                             std::string::npos)
                  mimeType = "image/jpeg";
                else if (iconUrl.find(".svg") !=
                         std::string::npos)
                  mimeType = "image/svg+xml";
                else if (iconUrl.find(".ico") !=
                         std::string::npos)
                  mimeType = "image/x-icon";
                else
                  mimeType = "image/png";
              }
              std::string b64 = base64Encode(
                  reinterpret_cast<const unsigned char*>(
                      iconData.data()),
                  iconData.size());
              // Construct new <link> tag with data URI
              std::string newTag =
                  "<link rel=\"icon\" href=\"data:" +
                  mimeType + ";base64," + b64 + "\"/>";
              result += newTag;
            } else {
              result += tag;  // if icon fetch fails, keep
                              // link as is
            }
          } else {
            result += tag;
          }
          i = tagEnd + 1;
          continue;
        } else {
          // Other <link> (manifest, alternate stylesheet,
          // etc.), just copy it
          result += tag;
          i = tagEnd + 1;
          continue;
        }
      } else if ((html.compare(i, 7, "<script") == 0) ||
                 (html.compare(i, 7, "<SCRIPT") == 0)) {
        size_t tagEnd = html.find('>', i);
        if (tagEnd == std::string::npos) break;
        std::string openTag =
            html.substr(i, tagEnd - i + 1);
        // Find the closing </script>
        size_t closeTagPos = html.find("</script", tagEnd);
        if (closeTagPos == std::string::npos)
          closeTagPos = html.find("</SCRIPT", tagEnd);
        size_t closeTagEnd =
            (closeTagPos != std::string::npos)
                ? html.find('>', closeTagPos)
                : std::string::npos;
        if (closeTagPos == std::string::npos ||
            closeTagEnd == std::string::npos) {
          // Malformed HTML (no closing script tag) – break
          // to avoid infinite loop
          break;
        }
        std::string fullTag =
            html.substr(i, closeTagEnd - i + 1);
        // Check for external script src
        size_t srcAttrPos = openTag.find("src");
        if (srcAttrPos != std::string::npos) {
          // Extract script URL
          std::string scriptUrl;
          size_t eq = openTag.find('=', srcAttrPos);
          size_t q = openTag.find_first_of("\"'", eq);
          if (eq != std::string::npos &&
              q != std::string::npos) {
            char qch = openTag[q];
            size_t endq = openTag.find(qch, q + 1);
            if (endq != std::string::npos) {
              scriptUrl =
                  openTag.substr(q + 1, endq - q - 1);
            }
          }
          if (!scriptUrl.empty()) {
            scriptUrl = resolveUrl(pageBaseUrl, scriptUrl);
            if (inlineJS) {
              // Inline the script content
              std::vector<char> jsData;
              std::string jsType;
              if (fetchUrl(scriptUrl, jsData, jsType)) {
                std::string jsCode(jsData.begin(),
                                   jsData.end());
                // Preserve type attribute if e.g.
                // type="module"
                std::string typeAttr;
                size_t typePos = openTag.find("type");
                if (typePos != std::string::npos) {
                  size_t eqt = openTag.find('=', typePos);
                  size_t qt =
                      openTag.find_first_of("\"'", eqt);
                  if (eqt != std::string::npos &&
                      qt != std::string::npos) {
                    char tq = openTag[qt];
                    size_t endtq = openTag.find(tq, qt + 1);
                    if (endtq != std::string::npos) {
                      typeAttr = openTag.substr(
                          qt, endtq - qt + 1);
                    }
                  }
                }
                result += "<script";
                if (!typeAttr.empty()) {
                  result += " " + typeAttr;
                }
                result += ">" + jsCode + "</script>";
              } else {
                // If script fetch fails, keep the original
                // tag referencing external src
                result += fullTag;
              }
            } else {
              // Save script to a local .js file and update
              // src reference
              std::vector<char> jsData;
              std::string jsType;
              if (fetchUrl(scriptUrl, jsData, jsType)) {
                std::string jsFileName =
                    sanitizeUrlToFilename(scriptUrl, ".js");
                if (visited.find(scriptUrl) ==
                    visited.end()) {
                  visited[scriptUrl] = jsFileName;
                  // Write the script file to output folder
                  std::ofstream ofs(
                      outputFolder + "/" + jsFileName,
                      std::ios::binary);
                  ofs.write(jsData.data(), jsData.size());
                  ofs.close();
                }
                // Reconstruct script tag with new src
                std::string newTag = "<script";
                // Preserve type attribute if present
                size_t typePos = openTag.find("type");
                if (typePos != std::string::npos) {
                  size_t eqt = openTag.find('=', typePos);
                  size_t qt =
                      openTag.find_first_of("\"'", eqt);
                  if (eqt != std::string::npos &&
                      qt != std::string::npos) {
                    char tq = openTag[qt];
                    size_t endtq = openTag.find(tq, qt + 1);
                    if (endtq != std::string::npos) {
                      std::string typeAttr = openTag.substr(
                          typePos, endtq - typePos + 1);
                      newTag += " " + typeAttr;
                    }
                  }
                }
                // Preserve async/defer attributes
                if (openTag.find("async") !=
                    std::string::npos)
                  newTag += " async";
                if (openTag.find("defer") !=
                    std::string::npos)
                  newTag += " defer";
                newTag += " src=\"" + visited[scriptUrl] +
                          "\"></script>";
                result += newTag;
              } else {
                result += fullTag;
              }
            }
          } else {
            result +=
                fullTag;  // if src attr is empty or not
                          // found, just copy the tag
          }
        } else {
          // Inline script (no src) – copy it verbatim
          result += fullTag;
        }
        i = closeTagEnd + 1;
        continue;
      } else if ((html.compare(i, 4, "<img") == 0) ||
                 (html.compare(i, 4, "<IMG") == 0)) {
        size_t tagEnd = html.find('>', i);
        if (tagEnd == std::string::npos) break;
        std::string tag = html.substr(i, tagEnd - i + 1);
        // Replace image src with base64 data URI
        size_t srcPos = tag.find("src");
        if (srcPos != std::string::npos) {
          std::string imgUrl;
          size_t eq = tag.find('=', srcPos);
          size_t q = tag.find_first_of("\"'", eq);
          if (eq != std::string::npos &&
              q != std::string::npos) {
            char qch = tag[q];
            size_t endq = tag.find(qch, q + 1);
            if (endq != std::string::npos) {
              imgUrl = tag.substr(q + 1, endq - q - 1);
            }
          }
          if (!imgUrl.empty() &&
              imgUrl.rfind("data:", 0) != 0) {
            imgUrl = resolveUrl(pageBaseUrl, imgUrl);
            std::vector<char> imgData;
            std::string imgType;
            if (fetchUrl(imgUrl, imgData, imgType)) {
              std::string mimeType;
              if (!imgType.empty()) {
                size_t sc = imgType.find(';');
                mimeType = (sc != std::string::npos
                                ? imgType.substr(0, sc)
                                : imgType);
              }
              if (mimeType.empty()) {
                // Guess MIME from file extension
                if (imgUrl.find(".png") !=
                    std::string::npos)
                  mimeType = "image/png";
                else if (imgUrl.find(".jpg") !=
                             std::string::npos ||
                         imgUrl.find(".jpeg") !=
                             std::string::npos)
                  mimeType = "image/jpeg";
                else if (imgUrl.find(".gif") !=
                         std::string::npos)
                  mimeType = "image/gif";
                else if (imgUrl.find(".svg") !=
                         std::string::npos)
                  mimeType = "image/svg+xml";
                else if (imgUrl.find(".webp") !=
                         std::string::npos)
                  mimeType = "image/webp";
                else
                  mimeType = "application/octet-stream";
              }
              std::string b64 = base64Encode(
                  reinterpret_cast<const unsigned char*>(
                      imgData.data()),
                  imgData.size());
              // Remove srcset attribute if present (to
              // avoid external references)
              size_t srcsetPos = tag.find("srcset");
              if (srcsetPos != std::string::npos) {
                size_t eqp = tag.find('=', srcsetPos);
                size_t q2 = tag.find('"', eqp);
                if (eqp != std::string::npos &&
                    q2 != std::string::npos) {
                  char qch2 = tag[q2];
                  size_t endq2 = tag.find(qch2, q2 + 1);
                  if (endq2 != std::string::npos) {
                    tag.erase(srcsetPos,
                              endq2 - srcsetPos + 1);
                  }
                }
              }
              // Replace src attribute value
              std::string dataUri =
                  "data:" + mimeType + ";base64," + b64;
              size_t quotePos =
                  tag.find_first_of("\"'", srcPos);
              if (quotePos != std::string::npos) {
                char qch = tag[quotePos];
                size_t endQuote =
                    tag.find(qch, quotePos + 1);
                if (endQuote != std::string::npos) {
                  tag.replace(quotePos + 1,
                              endQuote - quotePos - 1,
                              dataUri);
                }
              }
            }
          }
        }
        result += tag;
        i = tagEnd + 1;
        continue;
      } else if ((html.compare(i, 7, "<iframe") == 0) ||
                 (html.compare(i, 7, "<IFRAME") == 0)) {
        size_t tagEnd = html.find('>', i);
        if (tagEnd == std::string::npos) break;
        std::string openTag =
            html.substr(i, tagEnd - i + 1);
        // Find closing </iframe> (if any content inside)
        size_t closePos = html.find("</iframe", tagEnd);
        if (closePos == std::string::npos)
          closePos = html.find("</IFRAME", tagEnd);
        size_t closeEnd = (closePos != std::string::npos)
                              ? html.find('>', closePos)
                              : std::string::npos;
        std::string innerContent;
        if (closePos != std::string::npos &&
            closeEnd != std::string::npos) {
          innerContent = html.substr(tagEnd + 1,
                                     closePos - tagEnd - 1);
        }
        // Process src attribute of iframe
        size_t srcAttrPos = openTag.find("src");
        if (srcAttrPos != std::string::npos) {
          std::string frameUrl;
          size_t eq = openTag.find('=', srcAttrPos);
          size_t q = openTag.find_first_of("\"'", eq);
          if (eq != std::string::npos &&
              q != std::string::npos) {
            char qch = openTag[q];
            size_t endq = openTag.find(qch, q + 1);
            if (endq != std::string::npos) {
              frameUrl =
                  openTag.substr(q + 1, endq - q - 1);
            }
          }
          if (!frameUrl.empty()) {
            frameUrl = resolveUrl(pageBaseUrl, frameUrl);
            if (frameUrl.find(baseDomain) !=
                    std::string::npos &&
                depth > 1) {
              // Recursively archive iframe content if same
              // domain and depth allows
              if (visited.find(frameUrl) == visited.end()) {
                std::string frameFile =
                    sanitizeUrlToFilename(frameUrl,
                                          ".html");
                visited[frameUrl] = frameFile;
                std::string subContent =
                    SingleFile(frameUrl, depth - 1);
                if (!subContent.empty()) {
                  std::ofstream ofs(outputFolder + "/" +
                                    frameFile);
                  ofs << subContent;
                  ofs.close();
                }
              }
              // Replace src with local file reference
              size_t q1 =
                  openTag.find_first_of("\"'", srcAttrPos);
              if (q1 != std::string::npos) {
                char qch = openTag[q1];
                size_t q2 = openTag.find(qch, q1 + 1);
                if (q2 != std::string::npos) {
                  openTag.replace(q1 + 1, q2 - q1 - 1,
                                  visited[frameUrl]);
                }
              }
            } else if (visited.find(frameUrl) !=
                       visited.end()) {
              // If already archived this frame URL
              size_t q1 =
                  openTag.find_first_of("\"'", srcAttrPos);
              if (q1 != std::string::npos) {
                char qch = openTag[q1];
                size_t q2 = openTag.find(qch, q1 + 1);
                if (q2 != std::string::npos) {
                  openTag.replace(q1 + 1, q2 - q1 - 1,
                                  visited[frameUrl]);
                }
              }
            }
            // (If external domain or depth limit reached,
            // we leave the src as-is; it likely won't load
            // offline.)
          }
        }
        // Reconstruct iframe tag (with possibly modified
        // src) and its content
        result += openTag;
        result += innerContent;
        if (closePos != std::string::npos) {
          result += "</iframe>";
          i = closeEnd + 1;
        } else {
          i = tagEnd + 1;
        }
        continue;
      } else if ((html.compare(i, 2, "<a") == 0) ||
                 (html.compare(i, 2, "<A") == 0)) {
        size_t tagEnd = html.find('>', i);
        if (tagEnd == std::string::npos) break;
        std::string tag = html.substr(i, tagEnd - i + 1);
        // Rewrite anchor href if it’s an internal link and
        // we are archiving deeper
        size_t hrefPos = tag.find("href");
        if (hrefPos != std::string::npos) {
          std::string linkUrl;
          size_t eq = tag.find('=', hrefPos);
          size_t q = tag.find_first_of("\"'", eq);
          if (eq != std::string::npos &&
              q != std::string::npos) {
            char qch = tag[q];
            size_t endq = tag.find(qch, q + 1);
            if (endq != std::string::npos) {
              linkUrl = tag.substr(q + 1, endq - q - 1);
            }
          }
          if (!linkUrl.empty() &&
              linkUrl.rfind("#", 0) !=
                  0 &&  // ignore fragment links
              linkUrl.rfind("javascript:", 0) !=
                  0 &&  // ignore JS links
              linkUrl.rfind("mailto:", 0) !=
                  0) {  // ignore email links
            linkUrl = resolveUrl(pageBaseUrl, linkUrl);
            if (linkUrl.find(baseDomain) !=
                    std::string::npos &&
                depth > 1) {
              // Internal link: fetch recursively if not
              // visited
              if (visited.find(linkUrl) == visited.end()) {
                std::string fileName =
                    sanitizeUrlToFilename(linkUrl, ".html");
                visited[linkUrl] = fileName;
                std::string subContent =
                    SingleFile(linkUrl, depth - 1);
                if (!subContent.empty()) {
                  std::ofstream ofs(outputFolder + "/" +
                                    fileName);
                  ofs << subContent;
                  ofs.close();
                }
              }
              // Point href to the local file
              size_t q1 = tag.find_first_of("\"'", hrefPos);
              if (q1 != std::string::npos) {
                char qch = tag[q1];
                size_t q2 = tag.find(qch, q1 + 1);
                if (q2 != std::string::npos) {
                  tag.replace(q1 + 1, q2 - q1 - 1,
                              visited[linkUrl]);
                }
              }
            } else if (visited.find(linkUrl) !=
                       visited.end()) {
              // If link was already archived by another
              // path
              size_t q1 = tag.find_first_of("\"'", hrefPos);
              if (q1 != std::string::npos) {
                char qch = tag[q1];
                size_t q2 = tag.find(qch, q1 + 1);
                if (q2 != std::string::npos) {
                  tag.replace(q1 + 1, q2 - q1 - 1,
                              visited[linkUrl]);
                }
              }
            }
            // (External links or depth limit: leave them
            // pointing to original URL)
          }
        }
        result += tag;
        i = tagEnd + 1;
        continue;
      }
    }
    // Default: copy character as is (for text content or
    // tags not handled above)
    result.push_back(html[i]);
    ++i;
  }
  return result;
}

int main(int argc, char* argv[]) {
  std::string url;
  unsigned int depth = 1;
  std::string
      outOverride;  // CLI override for base output
                    // directory (parent of the slug)

  // Parse command-line flags
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("--url=", 0) == 0) {
      url = arg.substr(6);
    } else if (arg.rfind("--depth=", 0) == 0) {
      try {
        int d = std::stoi(arg.substr(8));
        depth = d < 1 ? 1u : static_cast<unsigned int>(d);
      } catch (...) {
        depth = 1;
      }
    } else if (arg == "--inline-js=false" ||
               arg == "--inline-js=0" ||
               arg == "--inline-js=no") {
      inlineJS = false;
    } else if (arg == "--inline-js=true" ||
               arg == "--inline-js=1" ||
               arg == "--inline-js=yes") {
      inlineJS = true;
    } else if (arg.rfind("--out=", 0) == 0) {
      outOverride = arg.substr(6);
    } else if (arg.rfind("--output-dir=", 0) == 0) {
      outOverride = arg.substr(13);
    }
  }

  if (url.empty()) {
    std::cerr << "Usage: " << argv[0]
              << " --url=<website URL> [--depth=<n>] "
                 "[--inline-js=(yes|no)]"
                 " [--out=<dir>|--output-dir=<dir>]\n";
    return 1;
  }

  curl_global_init(CURL_GLOBAL_ALL);

  // Determine the base domain (host) for the input URL
  // (used to restrict link following)
  size_t schemePos = url.find("://");
  if (schemePos != std::string::npos) {
    size_t hostPos = schemePos + 3;
    size_t slashPos = url.find('/', hostPos);
    baseDomain =
        (slashPos == std::string::npos)
            ? url.substr(hostPos)
            : url.substr(hostPos, slashPos - hostPos);
  } else {
    baseDomain = url;
  }

  // Base output directory precedence:
  // 1) --out / --output-dir (CLI)
  // 2) SF_OUT_DIR (env var)
  // 3) default "out/singlefile"
  std::string baseOutDir;
  if (!outOverride.empty()) {
    baseOutDir = outOverride;
  } else if (const char* envDir = std::getenv("SF_OUT_DIR");
             envDir && *envDir) {
    baseOutDir = envDir;
  } else {
    baseOutDir = "out/singlefile";
  }

  // Prepare output directory:
  // <baseOutDir>/<sanitized_main_url>/
  std::string mainSlug = sanitizeUrlToFilename(
      url, "");  // no extension for folder name
  std::string outDirPath = baseOutDir;
  if (!outDirPath.empty() && outDirPath.back() == '/')
    outDirPath.pop_back();
  outDirPath += "/" + mainSlug;

  // Create the output directory structure
  std::string mkdirCommand =
      std::string("mkdir -p \"") + outDirPath + "\"";
  if (system(mkdirCommand.c_str()) != 0) {
    std::cerr << "Warning: Failed to create directory "
              << outDirPath << "\n";
  }
  outputFolder = outDirPath;

  // Mark the main page as visited with filename
  // "index.html"
  visited[url] = "index.html";

  // Archive the main page (and linked pages up to depth)
  std::string mainContent = SingleFile(url, depth);

  // Save the main page content to index.html
  std::ofstream outFile(outputFolder + "/index.html");
  if (!mainContent.empty()) {
    outFile << mainContent;
  }
  outFile.close();

  curl_global_cleanup();

  std::cout << "Saved: " << outputFolder << "/index.html"
            << std::endl;
  return 0;
}
