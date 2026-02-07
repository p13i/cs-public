# cs/net/proto/codegen/BUILD
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
    data = ["//:.clang-format"],
    deps = [
        ":codegen_lib",
        "//cs/devtools:enforce_header_guards_lib",
        "//cs/devtools:filenames_in_comments_lib",
    ],
)

filegroup(
    name = "testdata",
    srcs = glob(["testdata/**"]),
)

py_test(
    name = "codegen_tests",
    srcs = [
        "codegen_tests.gpt.py",
        "tests/golden_utils.py",
        "tests/refresh_goldens.py",
        "tests/test_cli_modes.py",
        "tests/test_generation_outputs.py",
        "tests/test_parser_and_validator.py",
        "tests/test_toobject_fromobject_generation.gpt.py",
    ],
    data = [
        ":codegen",
        ":testdata",
        "//:.clang-format",
    ],
    env = {
        "PYTHONHASHSEED": "0",
    },
    main = "codegen_tests.gpt.py",
    deps = [":codegen_lib"],
)
