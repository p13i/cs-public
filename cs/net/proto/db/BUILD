# cs/net/proto/db/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "field_path_builder",
    hdrs = ["field_path_builder.gpt.hh"],
    deps = [],
)

cc_library(
    name = "query_helpers",
    hdrs = ["query_helpers.gpt.hh"],
    visibility = ["//visibility:public"],
    deps = [
        ":field_path_builder",
        "//cs/net/proto/db/protos:database.proto",
    ],
)

cc_library(
    name = "query_view",
    hdrs = ["query_view.gpt.hh"],
    visibility = ["//visibility:public"],
    deps = [
        ":field_path_builder",
        ":query_helpers",
        "//cs:log",
        "//cs:result",
        "//cs/apps/database-service:rpc.gpt",
        "//cs/net/proto:api",
        "//cs/net/proto/db/protos:database.proto",
        "//cs/net/rpc",
    ],
)

cc_library(
    name = "db",
    srcs = ["db.cc"],
    hdrs = [
        "db.hh",
        "field_path_builder.gpt.hh",
        "query_view.gpt.hh",
    ],
    deps = [
        ":field_path_builder",
        ":query_view",
        "//cs:log",
        "//cs:result",
        "//cs/fs",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/util:fmt",
    ],
)
