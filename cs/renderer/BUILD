package(default_visibility = ["//visibility:public"])

cc_library(
    name = "film",
    hdrs = ["film.hh"],
    deps = [
        ":pixel",
    ],
)

cc_library(
    name = "pixel",
    hdrs = ["pixel.hh"],
    deps = [
        "//cs:log",
    ],
)

cc_library(
    name = "rainbow",
    srcs = ["rainbow.cc"],
    hdrs = ["rainbow.h"],
    deps = [":film"],
)

cc_test(
    name = "rainbow_test",
    srcs = ["rainbow_test.cc"],
    deps = [
        ":rainbow",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "scene",
    srcs = ["scene.cc"],
    hdrs = ["scene.hh"],
    deps = [
        "//cs/renderer/geo:dist",
        "//cs/renderer/shapes:shape",
    ],
)

cc_library(
    name = "scene_renderer",
    srcs = ["scene_renderer.cc"],
    hdrs = ["scene_renderer.hh"],
    deps = [
        ":film",
        ":scene",
        "//cs/renderer/geo:dist",
        "//cs/renderer/geo:dot",
        "//cs/renderer/geo:point2",
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/linalg:transform",
        "//cs/renderer/math:constants",
        "//cs/renderer/numbers:clamp",
        "//cs/renderer/numbers:map_value",
        "//cs/renderer/shapes:shape",
        "//cs/util:parallel",
    ],
)

cc_test(
    name = "scene_renderer_test",
    srcs = ["scene_renderer_test.cc"],
    deps = [
        ":film",
        ":scene",
        ":scene_renderer",
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/shapes:plane",
        "//cs/renderer/shapes:sphere",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "sphere_renderer",
    srcs = ["sphere_renderer.cc"],
    hdrs = ["sphere_renderer.hh"],
    deps = [
        ":film",
        "//cs:result",
        "//cs/renderer/geo:dist",
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/math:constants",
        "//cs/renderer/numbers:clamp",
        "//cs/renderer/numbers:map_value",
        "//cs/renderer/shapes:sphere",
    ],
)

cc_test(
    name = "sphere_renderer_test",
    srcs = ["sphere_renderer_test.cc"],
    deps = [
        ":sphere_renderer",
        "@googletest//:gtest_main",
    ],
)
