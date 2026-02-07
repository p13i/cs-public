# cs/devtools/BUILD
load("@devtools_requirements//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

package(default_visibility = ["//visibility:public"])

py_binary(
    name = "add_acls",
    srcs = ["add_acls.py"],
)

py_binary(
    name = "newfile",
    srcs = ["newfile.py"],
)

py_binary(
    name = "enforce_header_guards",
    srcs = ["enforce_header_guards.py"],
    main = "enforce_header_guards.py",
)

py_library(
    name = "enforce_header_guards_lib",
    srcs = ["enforce_header_guards.py"],
)

py_binary(
    name = "move_cc",
    srcs = ["move_cc.py"],
    main = "move_cc.py",
)

py_binary(
    name = "parse_logs",
    srcs = ["parse_logs.py"],
    main = "parse_logs.py",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "check_visibility",
    srcs = ["check_visibility.py"],
    python_version = "PY3",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "lint_makefile",
    srcs = ["lint_makefile.py"],
    python_version = "PY3",
)

py_binary(
    name = "update_env",
    srcs = ["update_env.py"],  # the script from above
    main = "update_env.py",
)

py_binary(
    name = "gha",
    srcs = ["gha.py"],
    main = "gha.py",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "embed_files",
    srcs = ["embed_files.py"],
    srcs_version = "PY3",
)

py_binary(
    name = "inline_using",
    srcs = ["inline_using.py"],
    main = "inline_using.py",
    python_version = "PY3",
)

py_binary(
    name = "publish_code",
    srcs = ["publish_code.py"],
    main = "publish_code.py",
)

py_binary(
    name = "filenames_in_comments",
    srcs = ["filenames_in_comments.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "filenames_in_comments_lib",
    srcs = ["filenames_in_comments.py"],
)

py_binary(
    name = "format_black",
    srcs = ["format_black.py"],
    main = "format_black.py",
    deps = [
        requirement("black"),
    ],
)

py_binary(
    name = "codegen_goldens",
    srcs = ["codegen_goldens.gpt.py"],
    main = "codegen_goldens.gpt.py",
    python_version = "PY3",
)

py_binary(
    name = "test_endpoints",
    srcs = ["test_endpoints.gpt.py"],
    main = "test_endpoints.gpt.py",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "load_generator",
    srcs = ["load_generator.py"],
    main = "load_generator.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "expand_system",
    srcs = ["expand_system.gpt.py"],
    main = "expand_system.gpt.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "generate_routing",
    srcs = ["generate_routing.gpt.py"],
    main = "generate_routing.gpt.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "test_expand_system",
    srcs = ["test_expand_system.gpt.py"],
    main = "test_expand_system.gpt.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "compute_service_hash",
    srcs = ["compute_service_hash.gpt.py"],
    main = "compute_service_hash.gpt.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)

py_test(
    name = "test_expand_system_unit",
    srcs = [
        "expand_system.gpt.py",
        "test_expand_system_unit.gpt.py",
    ],
    main = "test_expand_system_unit.gpt.py",
    python_version = "PY3",
    deps = [
        requirement("pyyaml"),
    ],
)

py_test(
    name = "test_publish_code_unit",
    srcs = [
        "publish_code.py",
        "test_publish_code_unit.gpt.py",
    ],
    main = "test_publish_code_unit.gpt.py",
    python_version = "PY3",
)

py_test(
    name = "test_parse_logs",
    srcs = [
        "parse_logs.py",
        "test_parse_logs.gpt.py",
    ],
    main = "test_parse_logs.gpt.py",
    python_version = "PY3",
)

py_test(
    name = "test_embed_files",
    srcs = [
        "embed_files.py",
        "test_embed_files.gpt.py",
    ],
    main = "test_embed_files.gpt.py",
    python_version = "PY3",
)

py_test(
    name = "test_inline_using",
    srcs = [
        "inline_using.py",
        "test_inline_using.gpt.py",
    ],
    main = "test_inline_using.gpt.py",
    python_version = "PY3",
)

py_test(
    name = "test_newfile",
    srcs = [
        "newfile.py",
        "test_newfile.gpt.py",
    ],
    main = "test_newfile.gpt.py",
    python_version = "PY3",
)

py_test(
    name = "test_lint_makefile",
    srcs = [
        "lint_makefile.py",
        "test_lint_makefile.gpt.py",
    ],
    main = "test_lint_makefile.gpt.py",
    python_version = "PY3",
)

exports_files(["requirements.txt"])
