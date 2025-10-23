load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cs/net/proto/codegen:proto_codegen.bzl", "proto_codegen_cc_library")

package(default_visibility = ["//visibility:public"])

proto_codegen_cc_library(
    name = "mnist_data.proto",
    decl = "mnist_data.proto.hh",
)

proto_codegen_cc_library(
    name = "neural_model.proto",
    decl = "neural_model.proto.hh",
)
