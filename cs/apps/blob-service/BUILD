# cs/apps/blob-service/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "blob_store",
    srcs = ["blob_store.gpt.cc"],
    hdrs = ["blob_store.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/fs",
    ],
)

cc_library(
    name = "rpc",
    srcs = ["rpc.gpt.cc"],
    hdrs = ["rpc.gpt.hh"],
    deps = [
        ":blob_store",
        "//cs:result",
        "//cs/apps/blob-service/protos:blob.proto",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/net/rpc",
        "//cs/util:base64",
        "//cs/util:time",
        "//cs/util:uuid",
    ],
)

cc_test(
    name = "blob_store_test",
    srcs = ["blob_store_test.gpt.cc"],
    deps = [
        ":blob_store",
        "//cs:result",
        "//cs/fs",
        "//cs/testing:extensions",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "rpc_test",
    srcs = ["rpc_test.gpt.cc"],
    deps = [
        ":blob_store",
        ":rpc",
        "//cs:result",
        "//cs/apps/blob-service/protos:blob.proto",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:client_mock",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/net/proto/db/protos:database.proto",
        "//cs/util:base64",
        "//cs/util:uuid",
        "//cs/util/di:context",
        "@googletest//:gtest_main",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.gpt.cc"],
    deps = [
        ":blob_store",
        ":rpc",
        "//cs:result",
        "//cs/apps/blob-service/protos:blob.proto",
        "//cs/apps/blob-service/protos:config.proto",
        "//cs/apps/common:health_endpoint",
        "//cs/net/http:web_app",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/parsers:arg_parser",
        "//cs/util/di:context",
    ],
)
