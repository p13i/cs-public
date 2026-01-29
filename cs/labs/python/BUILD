# cs/labs/python/BUILD
package(default_visibility = ["//visibility:public"])

py_library(
    name = "hello_world_gpt_py",
    srcs = ["hello_world.gpt.py"],
)

py_test(
    name = "hello_world_test_gpt_py",
    srcs = ["hello_world_test.gpt.py"],
    data = [
        "BUILD",
        "acl.json",
    ],
    main = "hello_world_test.gpt.py",
    deps = [":hello_world_gpt_py"],
)
