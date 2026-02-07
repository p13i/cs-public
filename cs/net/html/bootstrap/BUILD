# cs/net/html/bootstrap/BUILD
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "bootstrap",
    srcs = ["dsl.cc"],
    hdrs = ["dsl.hh"],
    deps = ["//cs/net/html:dom"],
)

cc_library(
    name = "proto_form",
    hdrs = ["proto_form.gpt.hh"],
    deps = [
        "//cs/net/html:dom",
        "//cs/net/proto/form:proto_form",
    ],
)
