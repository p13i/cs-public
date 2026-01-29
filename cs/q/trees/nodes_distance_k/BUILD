# cs/q/trees/nodes_distance_k/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "nodes_distance_k",
    srcs = ["nodes_distance_k.cc"],
    hdrs = ["nodes_distance_k.hh"],
    deps = [
        "//cs/q/queue",
        "//cs/q/stack",
        "//cs/q/trees:node",
    ],
)

cc_test(
    name = "nodes_distance_k_test",
    srcs = ["nodes_distance_k_test.cc"],
    deps = [
        ":nodes_distance_k",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "nodes_distance_k_test_gpt",
    srcs = ["nodes_distance_k_test.gpt.cc"],
    deps = [
        ":nodes_distance_k",
        "@googletest//:gtest_main",
    ],
)
