# cs/apps/scribe-service/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "blob_client",
    srcs = ["blob_client.gpt.cc"],
    hdrs = ["blob_client.gpt.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/blob-service/protos:blob.proto",
        "//cs/net/http:client",
    ],
)

cc_library(
    name = "scribe_context",
    hdrs = ["scribe_context.gpt.hh"],
    deps = ["//cs/net/proto/db:client"],
)

cc_library(
    name = "ui",
    srcs = ["ui.gpt.cc"],
    hdrs = ["ui.gpt.hh"],
    deps = [
        ":blob_client",
        ":scribe_context",
        "//cs:result",
        "//cs/apps/common:website_nav",
        "//cs/apps/scribe-service/protos:job.proto",
        "//cs/apps/scribe-service/protos:upload.proto",
        "//cs/apps/task-queue-service:task_client.gpt",
        "//cs/net/html:dom",
        "//cs/net/html:escape",
        "//cs/net/html/bootstrap:proto_form",
        "//cs/net/http:handler",
        "//cs/net/http:status",
        "//cs/net/proto/db:client",
        "//cs/net/proto/db:query_helpers",
        "//cs/net/proto/db:query_view",
        "//cs/util:base64",
        "//cs/util:string",
        "//cs/util:uuid",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.gpt.cc"],
    deps = [
        ":scribe_context",
        ":ui",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/scribe-service/protos:config.proto",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:validators",
        "//cs/net/proto/db:client",
        "//cs/parsers:arg_parser",
    ],
)
