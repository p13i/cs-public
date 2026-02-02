# cs/apps/mobile/BUILD
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "app_sources",
    srcs = glob(
        [
            "*.json",
            "*.tsx",
            "*.js",
            "*.config.js",
            "tools/**",
            "__tests__/**",
        ],
        allow_empty = True,
        exclude = [
            "dist/**",
            "*.log",
        ],
    ),
)

sh_binary(
    name = "expo_start",
    srcs = ["tools/run_npm_script.sh"],
    args = ["start"],
    data = [":app_sources"],
)

sh_binary(
    name = "expo_ios",
    srcs = ["tools/run_npm_script.sh"],
    args = ["ios"],
    data = [":app_sources"],
)

sh_binary(
    name = "expo_android",
    srcs = ["tools/run_npm_script.sh"],
    args = ["android"],
    data = [":app_sources"],
)

sh_binary(
    name = "expo_web",
    srcs = ["tools/run_npm_script.sh"],
    args = ["web"],
    data = [":app_sources"],
)

sh_test(
    name = "tests",
    srcs = ["tools/run_npm_script.sh"],
    args = ["test"],
    data = [":app_sources"],
    env = {"CI": "1"},
)
