# cs/apps/prober/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prober",
    srcs = ["prober.gpt.cc"],
    hdrs = ["prober.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/prober/protos:probes.proto",
        "//cs/fs",
        "//cs/net/http:client",
        "//cs/net/json:parsers",
        "//cs/util:fmt",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/prober",
        "//cs/apps/prober/protos:config.proto",
        "//cs/apps/prober/protos:probes.proto",
        "//cs/fs",
        "//cs/net/proto:validators",
        "//cs/parsers:arg_parser",
    ],
)

cc_test(
    name = "prober_test",
    srcs = ["prober_test.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/prober",
        "//cs/apps/prober/protos:probes.proto",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:server",
        "//cs/testing:extensions",
        "@googletest//:gtest_main",
    ],
)
