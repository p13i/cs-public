# cs/q/trees/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "node",
    hdrs = ["node.hh"],
)

cc_library(
    name = "iterate",
    srcs = [
        "iterate/in_order.cc",
        "iterate/post_order.cc",
        "iterate/pre_order.cc",
    ],
    hdrs = [
        "iterate.hh",
        "node.hh",
    ],
    deps = ["//cs/q/stack"],
)

cc_test(
    name = "iterate_in_order_test",
    srcs = ["iterate/in_order_test.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_in_order_test_gpt",
    srcs = ["iterate/in_order_test.gpt.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_post_order_test",
    srcs = ["iterate/post_order_test.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_post_order_test_gpt",
    srcs = ["iterate/post_order_test.gpt.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_pre_order_test",
    srcs = ["iterate/pre_order_test.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_pre_order_test_gpt",
    srcs = ["iterate/pre_order_test.gpt.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iterate_base_test",
    srcs = ["iterate_base_test.gpt.cc"],
    deps = [
        ":iterate",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "view",
    srcs = [
        "view/from_left.cc",
        "view/from_right.cc",
        "view/from_top.cc",
    ],
    hdrs = [
        "node.hh",
        "view.hh",
    ],
    deps = [
        "//cs/q/queue",
    ],
)

cc_library(
    name = "traverse",
    srcs = [
        "traverse/in_order.cc",
        "traverse/post_order.cc",
        "traverse/pre_order.cc",
    ],
    hdrs = [
        "node.hh",
        "traverse.hh",
    ],
)

cc_test(
    name = "view_from_left_test",
    srcs = [
        "view/from_left_test.cc",
    ],
    deps = [
        ":view",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "view_from_right_test",
    srcs = [
        "view/from_right_test.cc",
    ],
    deps = [
        ":view",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "view_from_top_test",
    srcs = [
        "view/from_top_test.cc",
    ],
    deps = [
        ":view",
        "@googletest//:gtest_main",
    ],
)
