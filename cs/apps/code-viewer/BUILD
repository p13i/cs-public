# cs/apps/code-viewer/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = ["ui.gpt.cc"],
    hdrs = ["ui.gpt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/common:domain_resolver",
        "//cs/apps/common:load_endpoint",
        "//cs/apps/common:website",
        "//cs/apps/common:website_nav",
        "//cs/net/html:dom",
        "//cs/net/html/bootstrap",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/util:context",
        "//cs/util:string",
        "//cs/util/di:context",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":ui",
        "//cs:log",
        "//cs:result",
        "//cs/apps/code-viewer/protos:config.proto",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/common:load_endpoint",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/load/balancer/protos:downstream.proto",
        "//cs/parsers:arg_parser",
    ],
)
