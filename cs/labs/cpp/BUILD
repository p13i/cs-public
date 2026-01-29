# cs/labs/cpp/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hello_lib",
    srcs = ["hello.cc"],
    hdrs = ["hello.h"],
)

cc_binary(
    name = "hello_world",
    srcs = ["main.cc"],
    deps = [":hello_lib"],
)

cc_test(
    name = "hello_world_test_gpt_cc",
    srcs = ["hello_world_test.gpt.cc"],
    deps = [":hello_lib"],
)
