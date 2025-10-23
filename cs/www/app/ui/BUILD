package(default_visibility = ["//visibility:public"])

genrule(
    name = "main_css_inc",
    srcs = ["main.css"],
    outs = ["main.css.inc"],
    cmd = "echo 'R\"CSS(' > $@ && cat $(location main.css) >> $@ && echo ')CSS\"' >> $@",
)

cc_library(
    name = "ui",
    srcs = ["ui.cc"],
    hdrs = [
        "main_css_inc",
        "ui.hh",
    ],
    deps = [
        "//cs:log",
        "//cs/net/html:dom",
        "//cs/net/html:minify",
        "//cs/net/http:request",
        "//cs/net/http:response",
        "//cs/net/http:web_app",
        "//cs/net/proto:api",
        "//cs/net/proto/db",
        "//cs/parsers",
        "//cs/www/app:scene_animator",
        "//cs/www/app/api:auth",
        "//cs/www/app/api:game",
        "//cs/www/app/context",
        "//cs/www/app/protos:acl.proto",
        "//cs/www/app/protos:auth.proto",
        "//cs/www/app/protos:context.proto",
        "//cs/www/app/protos:game.proto",
        "//cs/www/app/protos:user.proto",
    ],
)
