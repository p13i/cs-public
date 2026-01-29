# cs/q/numeronym/BUILD
package(default_visibility = ["//visibility:public"])

cc_test(
    name = "numeronym_test",
    srcs = [
        "numeronym_test.cc",
    ],
    deps = [
        ":numeronym",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "numeronym",
    srcs = ["numeronym.c"],
    hdrs = ["numeronym.h"],
    deps = ["//cs/q/stack"],
)
