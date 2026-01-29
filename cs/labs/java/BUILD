# cs/labs/java/BUILD
package(default_visibility = ["//visibility:public"])

java_library(
    name = "hello_lib",
    srcs = ["HelloWorld.java"],
)

java_test(
    name = "hello_world_test",
    srcs = ["HelloWorldTest.java"],
    main_class = "HelloWorldTest",
    use_testrunner = False,
    deps = [":hello_lib"],
)
