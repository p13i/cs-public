# cs/apps/common/BUILD
# cs/apps/common/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//cs/devtools:embed_files.bzl", "embed_files_cc")
load("//cs/net/load/balancer:routing.bzl", "routing_json")

package(default_visibility = ["//visibility:public"])

routing_json(
    name = "routing",
    out = "routing.json",
    system_yml = "//:system.gpt.yml",
)

embed_files_cc(
    name = "routing_embedded",
    srcs = [":routing"],
    prefix = "routing",
)

cc_library(
    name = "domain_resolver",
    srcs = ["domain_resolver.gpt.cc"],
    hdrs = ["domain_resolver.gpt.hh"],
    deps = [
        ":routing_embedded",
        "//cs:result",
        "//cs/net/http:request",
        "//cs/net/load/balancer/protos:config.proto",
        "//cs/util:embedded_files",
    ],
)

cc_test(
    name = "domain_resolver_test",
    srcs = ["domain_resolver_test.gpt.cc"],
    deps = [
        ":routing_embedded",
        "//cs:result",
        "//cs/net/load/balancer/protos:config.proto",
        "//cs/util:embedded_files",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "website",
    srcs = ["website.gpt.cc"],
    hdrs = ["website.gpt.hh"],
    deps = [
        "//cs/apps/common/protos:website.proto",
        "//cs/net/html:dom",
        "//cs/net/html:minify",
        "//cs/net/html/bootstrap",
        "//cs/util:context",
    ],
)

cc_library(
    name = "website_nav",
    srcs = ["website_nav.gpt.cc"],
    hdrs = ["website_nav.gpt.hh"],
    deps = [
        "//cs/apps/common:website",
    ],
)

cc_library(
    name = "load_endpoint",
    hdrs = ["load_endpoint.gpt.hh"],
    deps = [
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:status",
        "//cs/net/load/balancer/protos:downstream.proto",
    ],
)

cc_library(
    name = "health_endpoint",
    hdrs = ["health_endpoint.hh"],
    deps = [
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:status",
        "//cs/util/di:context",
    ],
)
