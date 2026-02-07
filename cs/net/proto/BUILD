# cs/net/proto/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "proto",
    hdrs = ["proto.hh"],
    visibility = ["//visibility:public"],
    deps = ["//cs:result"],
)

cc_library(
    name = "validators",
    hdrs = ["validators.gpt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/parsers",
    ],
)

cc_test(
    name = "validators_test",
    srcs = ["validators_test.gpt.cc"],
    deps = [
        ":validators",
        "//cs:result",
        "@googletest//:gtest_main",
    ],
)
