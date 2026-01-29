# cs/q/strobogrammatic/BUILD
load("@rules_python//python:defs.bzl", "py_library", "py_test")

package(default_visibility = ["//visibility:public"])

# Library for the solution code
py_library(
    name = "solution",
    srcs = ["solution.py"],
    visibility = ["//visibility:public"],
)

# Unit tests for the solution
py_test(
    name = "solution_test",
    srcs = ["solution_test.py"],
    imports = ["."],  # allow test to do "import solution"
    deps = [
        ":solution",
    ],
)
