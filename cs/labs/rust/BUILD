# cs/labs/rust/BUILD
load(
    "@rules_rust//rust:defs.bzl",
    "rust_binary",
    "rust_library",
    "rust_test",
)

package(default_visibility = ["//visibility:public"])

rust_library(
    name = "hello_lib",
    srcs = ["lib.rs"],
)

rust_binary(
    name = "hello_world",
    srcs = ["main.rs"],
    deps = [":hello_lib"],
)

rust_test(
    name = "hello_world_test_gpt_rs",
    srcs = ["hello_world_test.gpt.rs"],
    data = [
        "BUILD",
        "hello_world_test.gpt.rs",
        "lib.rs",
        "main.rs",
    ],
    deps = [":hello_lib"],
)
