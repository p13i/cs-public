# cs/apps/common/protos/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cs/net/proto/codegen:proto_codegen.bzl", "proto_codegen_cc_library")

package(default_visibility = ["//visibility:public"])

proto_codegen_cc_library(
    name = "website.proto",
    decl = "website.proto.hh",
)
