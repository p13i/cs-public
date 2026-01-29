# cs/q/kth_largest_value/BUILD
package(default_visibility = ["//visibility:public"])

cc_test(
    name = "kth_largest_value_test",
    srcs = [
        "kth_largest_value_test.cc",
    ],
    deps = [
        ":kth_largest_value",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "kth_largest_value",
    srcs = ["kth_largest_value.c"],
    hdrs = ["kth_largest_value.h"],
    deps = ["//cs/q/priority_queue"],
)
