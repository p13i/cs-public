# cs/ai/gpt/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/ai/gpt:gpt_core",
        "//cs/ai/gpt:wikipedia",
        "//cs/ai/gpt/protos:config.proto",
        "//cs/parsers:arg_parser",
        "@curl",
    ],
)

cc_library(
    name = "gpt_core",
    srcs = ["main.cc"],
    hdrs = ["gpt_core.hh"],
    copts = ["-DGPT_NO_MAIN"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/ai/gpt:wikipedia",
        "//cs/ai/gpt/protos:config.proto",
        "//cs/parsers:arg_parser",
        "@curl",
    ],
)

cc_library(
    name = "wikipedia",
    srcs = ["wikipedia.cc"],
    hdrs = ["wikipedia.hh"],
    visibility = ["//visibility:public"],
    deps = [
        "//cs:log",
        "//cs/net/http:client",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
    ],
)

cc_binary(
    name = "download_wikipedia",
    srcs = ["download_wikipedia.cc"],
    deps = [
        "//cs:log",
        "//cs/ai/gpt:wikipedia",
        "//cs/fs",
        "//cs/parsers",
        "//cs/util:fmt",
    ],
)
