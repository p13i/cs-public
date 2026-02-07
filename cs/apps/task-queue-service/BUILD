# cs/apps/task-queue-service/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "task_client.gpt",
    srcs = ["task_client.gpt.cc"],
    hdrs = ["task_client.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/message-queue:rpc",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/apps/task-queue-service/protos:task.gpt.proto",
        "//cs/net/json:parsers",
        "//cs/net/proto:validators",
        "//cs/net/rpc",
        "//cs/util:time",
        "//cs/util:uuid",
    ],
)

cc_library(
    name = "task_worker.gpt",
    srcs = ["task_worker.gpt.cc"],
    hdrs = ["task_worker.gpt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/message-queue:rpc",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/apps/task-queue-service/protos:task.gpt.proto",
        "//cs/net/proto:validators",
        "//cs/net/rpc",
        "//cs/util:fmt",
    ],
)

cc_library(
    name = "rpc.gpt",
    srcs = ["rpc.gpt.cc"],
    hdrs = ["rpc.gpt.hh"],
    deps = [
        ":task_client.gpt",
        "//cs:result",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/apps/task-queue-service/protos:task_rpc.proto",
        "//cs/net/json:parsers",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/net/rpc",
    ],
)

cc_binary(
    name = "main",
    srcs = ["service_main.gpt.cc"],
    deps = [
        ":rpc.gpt",
        ":task_worker.gpt",
        "//cs:log",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/task-queue-service/protos:config.proto",
        "//cs/net/http:web_app",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/parsers:arg_parser",
        "//cs/util/di:context",
    ],
)

cc_binary(
    name = "worker_main",
    srcs = ["worker_main.gpt.cc"],
    deps = [
        ":task_worker.gpt",
        "//cs:log",
        "//cs:result",
        "//cs/apps/task-queue-service/protos:config.proto",
        "//cs/net/proto:validators",
        "//cs/parsers:arg_parser",
    ],
)

cc_test(
    name = "task_registry_test",
    srcs = ["task_registry_test.gpt.cc"],
    deps = [
        ":task_worker.gpt",
        "//cs:result",
        "//cs/net/json:parsers",
        "//cs/testing:extensions",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "task_contract_test",
    srcs = ["task_contract_test.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/task-queue-service/protos:task.gpt.proto",
        "//cs/net/json:parsers",
        "//cs/testing:extensions",
        "//cs/util:uuid",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "task_proto_parse_test",
    srcs = ["task_proto_parse_test.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/task-queue-service/protos:task.gpt.proto",
        "//cs/net/json:parsers",
        "//cs/testing:extensions",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "task_client_test",
    srcs = ["task_client_test.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/message-queue/protos:queue.proto",
        "//cs/apps/task-queue-service/protos:task.gpt.proto",
        "//cs/net/json:parsers",
        "//cs/testing:extensions",
        "//cs/util:time",
        "//cs/util:uuid",
        "@googletest//:gtest_main",
    ],
)
