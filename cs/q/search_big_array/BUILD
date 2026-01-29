# cs/q/search_big_array/BUILD
package(default_visibility = ["//visibility:public"])

cc_test(
    name = "search_big_array_test",
    srcs = [
        "search_big_array_test.cc",
    ],
    deps = [
        ":search_big_array",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "search_big_array",
    srcs = ["search_big_array.c"],
    hdrs = ["search_big_array.h"],
)
