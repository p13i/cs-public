# cs/net/load/balancer/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
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
    name = "proxy_environment",
    hdrs = ["proxy_environment.gpt.hh"],
    deps = ["//cs/util/di:constant_dep"],
)

cc_library(
    name = "balancer",
    srcs = ["balancer.cc"],
    hdrs = ["balancer.hh"],
    deps = [
        ":routing_embedded",
        "//cs:result",
        "//cs/net/html:dom",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:status",
        "//cs/net/http:web_app",
        "//cs/net/load/balancer/protos:downstream.proto",
        "//cs/util:fmt",
        "//cs/util:random",
        "//cs/util:timeit",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.gpt.cc"],
    hdrs = ["server.hh"],
    deps = [
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/load-balancer:routing_embedded",
        "//cs/apps/service-registry/protos:service.proto",
        "//cs/net/html:dom",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:status",
        "//cs/net/http:web_app",
        "//cs/net/load/balancer",
        "//cs/net/load/balancer:proxy_environment",
        "//cs/net/load/balancer/protos:config.proto",
        "//cs/net/load/balancer/protos:downstream.proto",
        "//cs/net/proto/db",
        "//cs/util:embedded_files",
        "//cs/util:fmt",
        "//cs/util:string",
        "//cs/util/di:context",
    ],
)
