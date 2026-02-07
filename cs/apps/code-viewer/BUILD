# cs/apps/code-viewer/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = ["ui.gpt.cc"],
    hdrs = ["ui.gpt.hh"],
    deps = [
        "//cs/apps/common/file-viewer:file_viewer",
        "//cs/net/http:request",
        "//cs/net/http:response",
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
        "//cs/apps/common:health_endpoint",
        "//cs/apps/common:load_endpoint",
        "//cs/apps/common/file-viewer/protos:config.proto",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/load/balancer/protos:downstream.proto",
        "//cs/parsers:arg_parser",
        "//cs/util:context",
    ],
)
