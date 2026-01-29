# cs/labs/go/BUILD
load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library", "go_test")

package(default_visibility = ["//visibility:public"])

go_library(
    name = "hello_lib",
    srcs = ["hello.go"],
    importpath = "cs/labs/go/hello",
    visibility = ["//visibility:public"],
)

go_binary(
    name = "hello_world",
    srcs = ["main.go"],
    importpath = "cs/labs/go/cmd/hello_world",
    deps = [":hello_lib"],
)

go_test(
    name = "hello_test_gpt_go",
    srcs = ["hello_test.gpt.go"],
    embed = [":hello_lib"],
)
