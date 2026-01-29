# cs/renderer/numbers/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "clamp",
    hdrs = ["clamp.hh"],
)

cc_library(
    name = "in_range",
    hdrs = ["in_range.hh"],
)

cc_library(
    name = "map_value",
    hdrs = ["map_value.hh"],
)

cc_test(
    name = "map_value_test",
    srcs = ["map_value_test.cc"],
    deps = [
        ":in_range",
        ":map_value",
        "//cs/renderer/precision:floats",
        "@googletest//:gtest_main",
    ],
)
