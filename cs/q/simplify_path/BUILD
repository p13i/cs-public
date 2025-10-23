package(default_visibility = ["//visibility:public"])

cc_test(
    name = "simplify_path_test_cc",
    srcs = [
        "simplify_path_test.cc",
    ],
    deps = [
        ":simplify_path_c",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "simplify_path_c",
    srcs = ["simplify_path.c"],
    hdrs = ["simplify_path.h"],
)

py_library(
    name = "simplify_path",
    srcs = ["simplify_path.py"],
    visibility = ["//visibility:public"],
)

py_test(
    name = "simplify_path_test",
    srcs = ["simplify_path_test.py"],
    imports = ["."],
    main = "simplify_path_test.py",
    deps = [":simplify_path"],
)

py_test(
    name = "simplify_path_test_chatgpt",
    srcs = ["simplify_path_test_chatgpt.py"],
    imports = ["."],
    main = "simplify_path_test_chatgpt.py",
    deps = [":simplify_path"],
)
