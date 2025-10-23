package(default_visibility = ["//visibility:public"])

cc_library(
    name = "auth",
    srcs = ["auth.cc"],
    hdrs = ["auth.hh"],
    deps = [
        "//cs:log",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/parsers",
        "//cs/util:uuid",
        "//cs/www/app/api:user",
        "//cs/www/app/context",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:user.proto",
    ],
)

cc_library(
    name = "user",
    srcs = ["user.cc"],
    hdrs = ["user.hh"],
    deps = [
        "//cs:log",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/net/rpc",
        "//cs/parsers",
        "//cs/util:uuid",
        "//cs/www/app/context",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:user.proto",
    ],
)

cc_library(
    name = "logs",
    srcs = ["logs.cc"],
    hdrs = ["logs.hh"],
    deps = [
        "//cs:log",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/net/proto/db:db_deferred_async",
        "//cs/parsers",
        "//cs/util:uuid",
        "//cs/www/app/context",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:log.proto",
        "//cs/www/app/protos:user.proto",
    ],
)

cc_library(
    name = "game",
    srcs = ["game.cc"],
    hdrs = ["game.hh"],
    deps = [
        "//cs:log",
        "//cs/net/proto:api",
        "//cs/www/app:scene_animator",
        "//cs/www/app/protos:game.proto",
    ],
)
