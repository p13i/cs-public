# cs/labs/csharp/BUILD
load("@rules_dotnet//dotnet:defs.bzl", "csharp_binary", "csharp_library", "csharp_test")

package(default_visibility = ["//visibility:public"])

csharp_library(
    name = "greeter_lib",
    srcs = ["Program.cs"],
    target_frameworks = ["net8.0"],
)

csharp_binary(
    name = "hello_world",
    srcs = ["Program.cs"],
    target_frameworks = ["net8.0"],
)

csharp_test(
    name = "hello_world_test_gpt_cs",
    srcs = ["hello_world_test.gpt.cs"],
    target_frameworks = ["net8.0"],
    deps = [":greeter_lib"],
)
