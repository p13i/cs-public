package(default_visibility = ["//visibility:public"])

cc_library(
    name = "fs",
    srcs = ["fs.cc"],
    hdrs = ["fs.hh"],
    deps = [
        "//cs:result",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "fs_test",
    srcs = ["fs_test.cc"],
    deps = [
        "//cs:result",
        "//cs/fs",
        "@googletest//:gtest_main",
    ],
)
