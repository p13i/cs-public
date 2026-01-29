# cs/net/rpc/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "rpc",
    srcs = ["rpc.cc"],
    hdrs = [
        "client.hh",
        "rpc.hh",
    ],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/net/http:client",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/json:object",
    ],
)
