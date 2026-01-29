# cs/apps/trycopilot.ai/text/fonts/BUILD
package(default_visibility = ["//visibility:public"])

# All custom fonts.

cc_library(
    name = "mono",
    srcs = ["mono.cc"],
    hdrs = ["mono.hh"],
)

cc_test(
    name = "mono_test",
    srcs = ["mono_test.gpt.cc"],
    deps = [
        ":mono",
        "@googletest//:gtest_main",
    ],
)
