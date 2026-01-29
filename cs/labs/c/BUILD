# cs/labs/c/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "hello_lib",
    srcs = ["hello_world.c"],
    hdrs = ["hello_world.h"],
)

cc_binary(
    name = "hello_world",
    srcs = ["main.c"],
    deps = [":hello_lib"],
)

cc_test(
    name = "hello_world_test_gpt_cc",
    srcs = ["hello_world_test.gpt.cc"],
    deps = [":hello_lib"],
)
