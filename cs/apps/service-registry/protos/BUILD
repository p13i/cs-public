# cs/apps/service-registry/protos/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cs/net/proto/codegen:proto_codegen.bzl", "proto_codegen_cc_library")

package(default_visibility = ["//visibility:public"])

proto_codegen_cc_library(
    name = "service.proto",
    decl = "service.proto.hh",
)

proto_codegen_cc_library(
    name = "docker.proto",
    decl = "docker.proto.hh",
)

proto_codegen_cc_library(
    name = "config.proto",
    decl = "config.proto.hh",
)

cc_test(
    name = "services_test",
    srcs = ["services_test.gpt.cc"],
    deps = [
        ":docker.proto",
        ":service.proto",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "services2_test",
    srcs = ["services2_test.gpt.cc"],
    deps = [
        ":docker.proto",
        ":service.proto",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "@googletest//:gtest_main",
    ],
)
