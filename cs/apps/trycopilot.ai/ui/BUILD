# cs/apps/trycopilot.ai/ui/BUILD
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = [
        "ui.cc",
    ],
    hdrs = [
        "doc_renderer.hh",
        "ui.hh",
    ],
    deps = [
        "//cs:log",
        "//cs:result",
        "//cs/apps/common:domain_resolver",
        "//cs/apps/common:website",
        "//cs/apps/common:website_nav",
        "//cs/apps/trycopilot.ai:scene_animator",
        "//cs/apps/trycopilot.ai/api:auth",
        "//cs/apps/trycopilot.ai/api:game",
        "//cs/apps/trycopilot.ai/protos:acl.proto",
        "//cs/apps/trycopilot.ai/protos:auth.proto",
        "//cs/apps/trycopilot.ai/protos:game.proto",
        "//cs/apps/trycopilot.ai/protos:ui.proto",
        "//cs/apps/trycopilot.ai/protos:user.proto",
        "//cs/apps/trycopilot.ai/protos:web_app_meta.proto",
        "//cs/net/html:dom",
        "//cs/net/html:minify",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto/db",
        "//cs/net/proto/db:database_base_url",
        "//cs/net/proto/form:proto_form",
        "//cs/net/rpc",
        "//cs/parsers",
        "//cs/util:context",
        "//cs/util:string",
    ],
)

cc_library(
    name = "doc_renderer",
    hdrs = ["doc_renderer.hh"],
    deps = [
        "//cs/apps/trycopilot.ai/protos:ui.proto",
    ],
)
