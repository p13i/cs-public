package(default_visibility = ["//visibility:public"])

cc_library(
    name = "object",
    hdrs = ["object.hh"],
    deps = [
        "//cs:result",
        "//cs/parsers",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "object_test",
    srcs = ["object_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "parsers",
    srcs = ["parsers.cc"],
    hdrs = ["parsers.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/net/json:object",
        "//cs/parsers",
        "//cs/util:fmt",
        "//cs/util:timeit",
    ],
)

cc_test(
    name = "parsers_test",
    srcs = ["parsers_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "serialize",
    srcs = ["serialize.cc"],
    hdrs = ["serialize.hh"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/util:fmt",
    ],
)

cc_test(
    name = "serialize_test",
    srcs = ["serialize_test.cc"],
    deps = [
        "//cs:result",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "@googletest//:gtest_main",
    ],
)
