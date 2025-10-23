package(default_visibility = ["//visibility:public"])

cc_library(
    name = "matrix4x4",
    srcs = ["matrix4x4.cc"],
    hdrs = ["matrix4x4.hh"],
    deps = [
        "//cs:result",
        "//cs/renderer/numbers:in_range",
        "//cs/util:fmt",
    ],
)

cc_library(
    name = "transform",
    srcs = ["transform.cc"],
    hdrs = ["transform.hh"],
    deps = [
        "//cs:result",
        "//cs/renderer/geo:cross",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/linalg:matrix4x4",
    ],
)

cc_library(
    name = "polar2cart",
    srcs = ["polar2cart.cc"],
    hdrs = ["polar2cart.hh"],
    deps = ["//cs/renderer/geo:point3"],
)
