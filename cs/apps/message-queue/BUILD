# cs/apps/message-queue/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "rpc",
    srcs = ["rpc.gpt.cc"],
    hdrs = ["rpc.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/net/proto/db/protos:database.proto",
        "//cs/net/rpc",
        "//cs/util:time",
        "//cs/util:uuid",
    ],
)

cc_test(
    name = "rpc_test",
    srcs = ["rpc_test.gpt.cc"],
    deps = [
        ":rpc",
        "//cs:result",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:client_mock",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/net/proto/db/protos:database.proto",
        "//cs/testing:extensions",
        "//cs/util:string",
        "//cs/util:uuid",
        "//cs/util/di:context",
        "@googletest//:gtest_main",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.gpt.cc"],
    deps = [
        ":rpc",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/message-queue/protos:config.proto",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/net/http:web_app",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/parsers:arg_parser",
        "//cs/util/di:context",
    ],
)
