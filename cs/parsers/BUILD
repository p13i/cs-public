# cs/parsers/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "parsers",
    srcs = ["parsers.cc"],
    hdrs = ["parsers.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/util:fmt",
    ],
)

cc_library(
    name = "arg_parser",
    hdrs = ["arg_parser.gpt.hh"],
    deps = [
        ":parsers",
        "//cs/net/json:object",
        "//cs/net/json:serialize",
        "//cs/net/proto",
        "//cs/net/proto/protos:meta.proto",
    ],
)

cc_test(
    name = "arg_parser_test",
    srcs = ["arg_parser_test.gpt.cc"],
    deps = [
        ":arg_parser",
        "//cs/apps/service-registry/protos:service.proto",
        "//cs/apps/trycopilot.ai/protos:matrix.proto",
        "//cs/apps/trycopilot.ai/protos:overland.proto",
        "//cs/net/load/simulator/protos:config.proto",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "parsers_test",
    srcs = ["parsers_test.gpt.cc"],
    deps = [
        ":parsers",
        "@googletest//:gtest_main",
    ],
)
