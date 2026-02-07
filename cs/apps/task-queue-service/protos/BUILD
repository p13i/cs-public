# cs/apps/task-queue-service/protos/BUILD
load("//cs/net/proto/codegen:proto_codegen.bzl", "proto_codegen_cc_library")

package(default_visibility = ["//visibility:public"])

proto_codegen_cc_library(
    name = "config.proto",
    decl = "config.proto.hh",
)

proto_codegen_cc_library(
    name = "task.gpt.proto",
    decl = "task.gpt.proto.hh",
)

proto_codegen_cc_library(
    name = "task_rpc.proto",
    decl = "task_rpc.proto.hh",
)
