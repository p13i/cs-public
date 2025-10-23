package(default_visibility = ["//visibility:public"])

cc_test(
    name = "stack_test",
    srcs = [
        "stack_test.cc",
    ],
    deps = [
        ":stack",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "stack",
    srcs = [
        "stack.cc",
    ],
    hdrs = [
        "stack.hh",
    ],
)
