# cs/apps/scribe-worker/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "whisper_task",
    srcs = ["whisper_task.gpt.cc"],
    hdrs = ["whisper_task.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/blob-service/protos:blob.proto",
        "//cs/apps/scribe-service/protos:job.proto",
        "//cs/fs",
        "//cs/net/http:client",
        "//cs/net/json:object",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/util:base64",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.gpt.cc"],
    deps = [
        ":whisper_task",
        "//cs:result",
        "//cs/apps/scribe-worker/protos:config.proto",
        "//cs/apps/task-queue-service:task_worker.gpt",
        "//cs/net/proto/db:client",
        "//cs/parsers:arg_parser",
    ],
)
