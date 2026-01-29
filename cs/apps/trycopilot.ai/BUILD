# cs/apps/trycopilot.ai/BUILD
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("//cs/devtools:embed_files.bzl", "embed_files_cc")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "fps_counter",
    hdrs = ["fps_counter.hh"],
)

# Exclude this library from the main build since the wasm_cc_binary
# below will compile the index.cc file with the emscripten SDK.
cc_library(
    name = "scene_animator",
    srcs = ["scene_animator.hh"],
    tags = ["manual"],
    deps = [
        "//cs/apps/trycopilot.ai/text/fonts:mono",
        "//cs/renderer:rainbow",
        "//cs/renderer:scene_renderer",
        "//cs/renderer/geo:point3",
        "//cs/renderer/geo:ray3",
        "//cs/renderer/geo:vector3",
        "//cs/renderer/linalg:transform",
        "//cs/renderer/numbers:map_value",
        "//cs/renderer/shapes:plane",
        "//cs/renderer/shapes:shape",
        "//cs/renderer/shapes:sphere",
        "//cs/util:timeit",
    ],
)

cc_test(
    name = "scene_animator_test_gpt_cc",
    srcs = ["scene_animator_test.gpt.cc"],
    deps = [
        "//cs:result",
        "//cs/apps/trycopilot.ai:scene_animator",
        "@googletest//:gtest_main",
    ],
)

cc_binary(
    name = "index",
    srcs = ["index.cc"],
    linkopts = [
        "-sASYNCIFY=1",
        "-sUSE_SDL=1",
        "-sALLOW_MEMORY_GROWTH",
        "-sEXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\", \"requestFullscreen\"]'",
        "-sFETCH=1",
        "-sASSERTIONS",
    ],
    # Exclude this library from the main build since the wasm_cc_binary
    # below will compile the index.cc file with the emscripten SDK.
    tags = ["manual"],
    deps = [
        "//cs/apps/trycopilot.ai:scene1",
        "//cs/apps/trycopilot.ai:scene_animator",
        "//cs/apps/trycopilot.ai/emscripten",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:game.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/apps/trycopilot.ai/protos:uuid.proto",
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/util:timeit",
    ],
)

wasm_cc_binary(
    name = "index.js",
    cc_target = ":index",
)

embed_files_cc(
    name = "index_embedded",
    srcs = [":index.js"],
    prefix = "indexjs",
)

cc_binary(
    name = "main",
    srcs = [
        "main.cc",
    ],
    data = [
        "//cs/apps/trycopilot.ai:index.js",
    ],
    deps = [
        ":index_embedded",
        "//cs:result",
        "//cs/apps/common:health_endpoint",
        "//cs/apps/common:load_endpoint",
        "//cs/apps/service-registry:rpc",
        "//cs/apps/trycopilot.ai:scene1",
        "//cs/apps/trycopilot.ai:scene_animator",
        "//cs/apps/trycopilot.ai/api:auth",
        "//cs/apps/trycopilot.ai/api:game",
        "//cs/apps/trycopilot.ai/api:prompt",
        "//cs/apps/trycopilot.ai/api:prompt_driver",
        "//cs/apps/trycopilot.ai/api:user",
        "//cs/apps/trycopilot.ai/protos:acl.proto",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:config.proto",
        "//cs/apps/trycopilot.ai/protos:context.proto",
        "//cs/apps/trycopilot.ai/protos:prompt.proto",
        "//cs/apps/trycopilot.ai/protos:prompt_driver.proto",
        "//cs/apps/trycopilot.ai/protos:ui.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/apps/trycopilot.ai/protos:uuid.proto",
        "//cs/apps/trycopilot.ai/ui",
        "//cs/fs",
        "//cs/net/http:favicon",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/json:object",
        "//cs/net/json:parsers",
        "//cs/net/json:serialize",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/net/rpc",
        "//cs/parsers:arg_parser",
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/util:context",
        "//cs/util:embedded_files",
        "//cs/util:timeit",
        "//cs/util:uuid",
        "@bazel_tools//tools/cpp/runfiles",
    ],
)

cc_library(
    name = "scene1",
    hdrs = ["scene1.hh"],
)
