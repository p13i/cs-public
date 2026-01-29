# cs/q/trees/vertical_order/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vertical_order_cc",
    srcs = ["vertical_order.cc"],
    hdrs = ["vertical_order.hh"],
    deps = ["//cs/q/trees:node"],
)

py_library(
    name = "vertical_order_py",
    srcs = ["vertical_order.py"],
    visibility = ["//visibility:public"],
)

py_test(
    name = "vertical_order_test_gpt_py",
    srcs = ["vertical_order_test.gpt.py"],
    imports = ["."],
    main = "vertical_order_test.gpt.py",
    deps = [":vertical_order_py"],
)
