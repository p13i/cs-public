# cs/apps/trycopilot.ai/api/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "auth",
    srcs = ["auth.cc"],
    hdrs = ["auth.hh"],
    deps = [
        "//cs:log",
        "//cs/apps/trycopilot.ai/api:user",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:api",
        "//cs/net/proto:validators",
        "//cs/net/proto/db",
        "//cs/parsers",
        "//cs/util:context",
        "//cs/util:uuid",
    ],
)

cc_library(
    name = "user",
    srcs = ["user.cc"],
    hdrs = ["user.hh"],
    deps = [
        "//cs:log",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/net/rpc",
        "//cs/parsers",
        "//cs/util:context",
        "//cs/util:uuid",
    ],
)

cc_library(
    name = "logs",
    srcs = ["logs.cc"],
    hdrs = ["logs.hh"],
    deps = [
        "//cs:log",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:log.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/parsers",
        "//cs/util:context",
        "//cs/util:uuid",
    ],
)

cc_test(
    name = "logs_test",
    srcs = ["logs_test.gpt.cc"],
    deps = [
        ":logs",
        "//cs/net/proto/db",
        "//cs/util:context",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "auth_test",
    srcs = ["auth_test.gpt.cc"],
    deps = [
        ":auth",
        ":user",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/net/proto/db",
        "//cs/util:uuid",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "game",
    srcs = ["game.cc"],
    hdrs = ["game.hh"],
    deps = [
        "//cs:log",
        "//cs/apps/trycopilot.ai:scene_animator",
        "//cs/apps/trycopilot.ai/protos:game.proto",
        "//cs/net/proto:api",
    ],
)

cc_library(
    name = "prompt",
    srcs = ["prompt.cc"],
    hdrs = ["prompt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/ai/gpt:gpt_core",
        "//cs/apps/trycopilot.ai/protos:prompt.proto",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/proto:api",
        "//cs/util:context",
    ],
)

cc_library(
    name = "prompt_driver",
    srcs = ["prompt_driver.cc"],
    hdrs = ["prompt_driver.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/trycopilot.ai/protos:prompt_driver.proto",
        "//cs/net/proto:api",
    ],
)
