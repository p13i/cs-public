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
    name = "use_usings",
    srcs = ["use_usings.py"],
    main = "use_usings.py",
    python_version = "PY3",
)

py_binary(
    name = "enforce_header_guards",
    srcs = ["enforce_header_guards.py"],
    main = "enforce_header_guards.py",
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
    name = "www",
    srcs = ["www.py"],
    python_version = "PY3",
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

cc_binary(
    name = "singlefile",
    srcs = ["singlefile.cc"],
    deps = [
        "@curl",
    ],
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
