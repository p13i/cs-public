# cs/apps/load-balancer/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")
load("//cs/devtools:embed_files.bzl", "embed_files_cc")
load("//cs/net/load/balancer:routing.bzl", "routing_json")

package(default_visibility = ["//visibility:public"])

# Generate routing.json with both prod and dev routes
routing_json(
    name = "routing",
    out = "routing.json",
    system_yml = "//:system.gpt.yml",
)

# Embed routing.json at compile time
embed_files_cc(
    name = "routing_embedded",
    srcs = [":routing"],
    prefix = "routing",
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":routing_embedded",
        "//cs:result",
        "//cs/net/load/balancer:server",
        "//cs/net/load/balancer/protos:config.proto",
        "//cs/net/proto:validators",
        "//cs/parsers:arg_parser",
        "//cs/util:context",
    ],
)

cc_test(
    name = "main_test",
    srcs = ["main_test.gpt.cc"],
    deps = [
        "//cs/net/load/balancer/protos:config.proto",
        "//cs/parsers:arg_parser",
        "@googletest//:gtest_main",
    ],
)
