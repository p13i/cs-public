# cs/net/rpc/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "rpc",
    srcs = ["rpc.cc"],
    hdrs = [
        "client.hh",
        "rpc.hh",
    ],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/json:object",
        "//cs/util/di:context",
    ],
)

cc_test(
    name = "extract_proto_from_request_test",
    srcs = ["extract_proto_from_request_test.gpt.cc"],
    deps = [
        ":rpc",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/rpc/protos:extract_test.proto",
        "@googletest//:gtest_main",
    ],
)
