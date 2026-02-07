# cs/apps/common/file-viewer/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "quoted_include_link",
    srcs = ["quoted_include_link.gpt.cc"],
    hdrs = ["quoted_include_link.gpt.hh"],
    deps = [
        "//cs/net/html:dom",
        "//cs/net/html:escape",
    ],
)

cc_library(
    name = "file_viewer",
    srcs = ["file_viewer.gpt.cc"],
    hdrs = ["file_viewer.gpt.hh"],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/common:domain_resolver",
        "//cs/apps/common:load_endpoint",
        "//cs/apps/common:website",
        "//cs/apps/common:website_nav",
        "//cs/apps/common/file-viewer:quoted_include_link",
        "//cs/net/html:dom",
        "//cs/net/html:escape",
        "//cs/net/html/bootstrap",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "//cs/util:context",
        "//cs/util:string",
        "//cs/util/di:context",
    ],
)

cc_test(
    name = "quoted_include_link_test",
    srcs = ["quoted_include_link_test.gpt.cc"],
    deps = [
        ":quoted_include_link",
        "@googletest//:gtest_main",
    ],
)
