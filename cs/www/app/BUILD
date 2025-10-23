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
        "//cs/www/app/text/fonts:mono",
    ],
)

cc_test(
    name = "scene_animator_test",
    srcs = ["scene_animator_test.cc"],
    deps = [
        "//cs:result",
        "//cs/www/app:scene_animator",
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
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/util:timeit",
        "//cs/www/app:scene1",
        "//cs/www/app:scene_animator",
        "//cs/www/app/emscripten",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:game.proto",
        "//cs/www/app/protos:user.proto",
        "//cs/www/app/protos:uuid.proto",
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
        "//cs/www/app:index.js",
    ],
    stamp = 1,
    deps = [
        ":index_embedded",
        "//cs:result",
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
        "//cs/renderer:film",
        "//cs/renderer:pixel",
        "//cs/util:timeit",
        "//cs/util:uuid",
        "//cs/www/app:scene1",
        "//cs/www/app:scene_animator",
        "//cs/www/app/api:auth",
        "//cs/www/app/api:game",
        "//cs/www/app/api:user",
        "//cs/www/app/context",
        "//cs/www/app/protos:acl.proto",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:context.proto",
        "//cs/www/app/protos:user.proto",
        "//cs/www/app/protos:uuid.proto",
        "//cs/www/app/ui",
        "@bazel_tools//tools/cpp/runfiles",
    ],
)

cc_library(
    name = "scene1",
    hdrs = ["scene1.hh"],
)

py_binary(
    name = "prober",
    srcs = ["prober.py"],
    data = [
        "probes.json",
    ],
)
