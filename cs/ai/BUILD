package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "mnist",
    srcs = ["mnist.cc"],
    deps = [
        "//cs/ai:neural_net",
        "//cs/ai/protos:mnist_data.proto",
        "//cs/ai/protos:neural_model.proto",
        "//cs/fs",
    ],
)

cc_library(
    name = "neural_net",
    srcs = ["neural_net.cc"],
    hdrs = ["neural_net.hh"],
    deps = [
        "//cs:log",
    ],
)
