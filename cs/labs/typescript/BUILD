# cs/labs/typescript/BUILD
package(default_visibility = ["//visibility:public"])

py_test(
    name = "hello_world_test_gpt_py",
    srcs = ["hello_world_test.gpt.py"],
    data = [
        "hello_world.gpt.ts",
        "node_globals.d.ts",
        "tsconfig.json",
    ],
    main = "hello_world_test.gpt.py",
)
