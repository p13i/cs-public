package(default_visibility = ["//visibility:public"])

cc_library(
    name = "proto",
    hdrs = ["proto.hh"],
    visibility = ["//visibility:public"],
    deps = ["//cs:result"],
)

cc_library(
    name = "api",
    srcs = ["api.cc"],
    hdrs = ["api.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/json:object",
    ],
)

cc_library(
    name = "validators",
    srcs = ["validators.cc"],
    hdrs = ["validators.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/parsers",
    ],
)
