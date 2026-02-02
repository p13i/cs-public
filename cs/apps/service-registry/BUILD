# cs/apps/service-registry/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "rpc",
    srcs = [
        "rpc.gpt.cc",
    ],
    hdrs = [
        "rpc.gpt.hh",
    ],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/service-registry/protos:service.proto",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto/db",
        "//cs/net/proto/db:database_base_url",
        "//cs/net/rpc",
        "//cs/util:context",
    ],
)

cc_binary(
    name = "main",
    srcs = [
        "main.gpt.cc",
        "service_registry_app.gpt.cc",
        "service_registry_app.gpt.hh",
    ],
    deps = [
        ":rpc",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/service-registry/protos:config.proto",
        "//cs/apps/service-registry/protos:docker.proto",
        "//cs/apps/service-registry/protos:service.proto",
        "//cs/fs",
        "//cs/net/http:web_app",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto/db",
        "//cs/net/proto/db:database_base_url",
        "//cs/parsers:arg_parser",
        "//cs/util:fmt",
        "//cs/util:random",
        "//cs/util:time",
        "//cs/util:uuid",
    ],
)

cc_test(
    name = "main_test",
    srcs = ["main_test.gpt.cc"],
    deps = [
        "//cs/apps/service-registry/protos:config.proto",
        "//cs/parsers:arg_parser",
        "@googletest//:gtest_main",
    ],
)
