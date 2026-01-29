# cs/q/queue/BUILD
package(default_visibility = ["//visibility:public"])

cc_test(
    name = "queue_test",
    srcs = [
        "queue_test.cc",
    ],
    deps = [
        ":queue",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "queue",
    srcs = [
        "queue.cc",
    ],
    hdrs = [
        "queue.hh",
    ],
)
