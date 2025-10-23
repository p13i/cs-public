package(default_visibility = ["//visibility:public"])

cc_library(
    name = "plane",
    srcs = ["plane.cc"],
    hdrs = ["plane.hh"],
    deps = [
        ":shape",
        "//cs/renderer/geo:dot",
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/precision:floats",
    ],
)

cc_test(
    name = "plane_test",
    srcs = ["plane_test.cc"],
    deps = [
        ":plane",
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/renderer/math:constants",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "shape",
    hdrs = ["shape.hh"],
    deps = [
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
    ],
)

cc_library(
    name = "sphere",
    srcs = ["sphere.cc"],
    hdrs = ["sphere.hh"],
    deps = [
        ":shape",
        "//cs/renderer/geo:dot",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/precision:floats",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "sphere_test",
    srcs = ["sphere_test.cc"],
    deps = [
        ":sphere",
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/renderer/math:constants",
        "@googletest//:gtest_main",
    ],
)
