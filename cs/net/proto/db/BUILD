package(default_visibility = ["//visibility:public"])

cc_library(
    name = "db",
    srcs = ["db.cc"],
    hdrs = ["db.hh"],
    deps = [
        "//cs:result",
        "//cs/fs",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/util:fmt",
    ],
)

cc_library(
    name = "db_deferred_async",
    hdrs = ["db_deferred_async.hh"],
    deps = [
        "//cs:result",
        "//cs/fs",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/util:fmt",
    ],
)
