# cs/net/html/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "escape",
    srcs = ["escape.gpt.cc"],
    hdrs = ["escape.gpt.hh"],
)

cc_test(
    name = "escape_test",
    srcs = ["escape_test.gpt.cc"],
    deps = [
        ":escape",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "dom",
    hdrs = ["dom.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/parsers",
    ],
)

cc_library(
    name = "minify",
    srcs = ["minify.gpt.cc"],
    hdrs = ["minify.hh"],
)

cc_test(
    name = "minify_test",
    srcs = ["minify_test.gpt.cc"],
    deps = [
        ":minify",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "dom_test",
    srcs = ["dom_test.gpt.cc"],
    deps = [
        ":dom",
        "@googletest//:gtest_main",
    ],
)
