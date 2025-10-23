package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//cs:result",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/util:timeit",
        "//cs/www/app:scene1",
        "//cs/www/app:scene_animator",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.hh"],
    deps = [
        ":request",
        ":response",
        "//cs:log",
        "//cs:result",
        "//cs/parsers",
        "//cs/util:timeit",
        "//cs/util:uuid",
    ],
)

cc_library(
    name = "parsers",
    srcs = ["parsers.cc"],
    hdrs = ["parsers.hh"],
    deps = [
        "//cs:result",
    ],
)

cc_library(
    name = "request",
    srcs = ["request.cc"],
    hdrs = ["request.hh"],
    deps = [
        "//cs:result",
        "//cs/net/http:parsers",
    ],
)

cc_library(
    name = "response",
    srcs = ["response.cc"],
    hdrs = ["response.hh"],
    deps = [
        ":status",
        "//cs:result",
        "//cs/net/http:parsers",
        "//cs/util:fmt",
    ],
)

cc_library(
    name = "status",
    hdrs = ["status.hh"],
)

cc_library(
    name = "web_app",
    hdrs = [
        "web_app.hh",
    ],
    deps = [
        ":request",
        ":response",
        ":server",
        "//cs:log",
        "//cs:result",
        "//cs/www/app/api:logs",
    ],
)

cc_library(
    name = "favicon",
    srcs = ["favicon.cc"],
    hdrs = ["favicon.hh"],
)
