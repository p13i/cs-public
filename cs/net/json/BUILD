# cs/net/json/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "object",
    hdrs = ["object.hh"],
    deps = [
        "//cs:result",
        "//cs/parsers",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "object_test",
    srcs = ["object_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:serialize",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parsers",
    srcs = ["parsers.cc"],
    hdrs = ["parsers.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/net/json:object",
        "//cs/parsers",
        "//cs/util:fmt",
        "//cs/util:timeit",
    ],
)

cc_test(
    name = "parsers_test",
    srcs = ["parsers_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "serialize",
    srcs = ["serialize.cc"],
    hdrs = ["serialize.hh"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "serialize_test",
    srcs = ["serialize_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "json_dsl",
    hdrs = ["json_dsl.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
    ],
)

cc_test(
    name = "absurd_json_test",
    srcs = ["absurd_json_test.gpt.cc"],
    deps = [
        "//cs/net/json:json_dsl",
    ],
)
