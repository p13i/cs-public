# cs/apps/database-service/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "rpc.gpt",
    srcs = ["rpc.gpt.cc"],
    hdrs = ["rpc.gpt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/fs",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto/db/protos:database.proto",
        "//cs/net/rpc",
        "//cs/util:context",
        "//cs/util:parallel",
        "//cs/util:uuid",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":rpc.gpt",
        "//cs:log",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/database-service/protos:config.proto",
        "//cs/net/http:web_app",
        "//cs/parsers:arg_parser",
        "//cs/util:context",
    ],
)
