# cs/renderer/audio/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sinusodal",
    srcs = ["sinusodal.cc"],
    hdrs = ["sinusodal.h"],
    deps = [
        "//cs/renderer/math:constants",
    ],
)

cc_test(
    name = "sinusodal_test",
    srcs = ["sinusodal_test.cc"],
    deps = [
        ":sinusodal",
        "//cs/renderer/precision:floats",
        "@googletest//:gtest_main",
    ],
)
