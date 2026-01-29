# cs/apps/cite.pub/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    data = [
    ],
    deps = [
        "//cs:result",
        "//cs/apps/cite.pub/protos:config.proto",
        "//cs/apps/cite.pub/protos:get_cached_page_request.gpt.proto",
        "//cs/apps/cite.pub/protos:lookup_mapping.proto",
        "//cs/apps/cite.pub/protos:save_resource_form.gpt.proto",
        "//cs/apps/common:health_endpoint",
        "//cs/fs",
        "//cs/net/html:dom",
        "//cs/net/http:favicon",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/net/proto/form:proto_form",
        "//cs/parsers:arg_parser",
        "//cs/util:context",
        "//cs/util:uuid",
    ],
)
