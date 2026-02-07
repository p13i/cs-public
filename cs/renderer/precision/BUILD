# cs/renderer/precision/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "floats",
    hdrs = ["floats.hh"],
)

cc_test(
    name = "floats_test",
    srcs = ["floats_test.gpt.cc"],
    deps = [
        ":floats",
        "@googletest//:gtest_main",
    ],
)
