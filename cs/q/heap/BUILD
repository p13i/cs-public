package(default_visibility = ["//visibility:public"])

cc_test(
    name = "heap_test",
    srcs = [
        "heap_test.cc",
    ],
    deps = [
        ":heap",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "heap",
    srcs = [
        "heap.c",
        "heap.cc",
    ],
    hdrs = [
        "heap.h",
        "heap.hh",
    ],
)
