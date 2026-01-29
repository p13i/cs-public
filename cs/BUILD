# cs/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "log",
    hdrs = ["log.hh"],
    deps = [
        "//cs/util:time",
    ],
)

cc_library(
    name = "result",
    hdrs = ["result.hh"],
    deps = [
        "//cs:log",
        "//cs/util:fmt",
    ],
)
