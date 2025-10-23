package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    data = [
    ],
    stamp = 1,
    deps = [
        "//cs:result",
        "//cs/fs",
        "//cs/net/http:favicon",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/util:timeit",
        "//cs/util:uuid",
    ],
)
