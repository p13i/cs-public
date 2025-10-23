package(default_visibility = ["//visibility:public"])

py_library(
    name = "codegen_lib",
    srcs = [
        "codegen_types.py",
        "constants.py",
        "generators.py",
        "helpers.py",
        "parser.py",
        "testing.py",
        "validator.py",
        "writers.py",
    ],
)

py_binary(
    name = "codegen",
    srcs = ["codegen.py"],
    deps = [":codegen_lib"],
)
