def _stem_from_decl(decl):
    # decl like "protos/foo.proto.hh" or "foo.proto.hh"
    bn = decl.rsplit("/", 1)[-1]
    if not bn.endswith(".proto.hh"):
        fail("Expected a '.proto.hh' file, got: %s" % decl)
    return bn[:-len(".proto.hh")]

def proto_codegen_cc_library(name, decl, deps = [], visibility = None):
    """
    Generates and compiles one proto unit.
    - name: logical name of the library to expose (e.g., "mnist_data")
    - decl: path to input .proto.hh (relative to this package)
    - deps: deps for the resulting cc_library
    - visibility: optional visibility list
    Creates:
      :<name>                -> cc_library with generated .cc + .hh
      :<name>_codegen        -> genrule (internal)
      :<name>_test_hdrs      -> filegroup exposing X_test.proto.hh
    """
    stem = _stem_from_decl(decl)
    out_cc = "gencode/%s.proto.cc" % stem
    out_hh = "gencode/%s.proto.hh" % stem
    out_test_cc = "gencode/%s.proto_test.cc" % stem

    native.genrule(
        name = name + "_codegen",
        srcs = [decl],
        tools = ["//cs/net/proto/codegen:codegen"],
        outs = [out_cc, out_hh, out_test_cc],
        # $@ expands to the *first* output; we only need its directory ($(@D))
        # $(location <label>) gives the absolute path inside the execroot.
        cmd = """
          $(location //cs/net/proto/codegen:codegen) --single \
            --in_abs $(location {proto}) \
            --in_rel {proto} \
            --out_cc $(@D)/{stem}.proto.cc \
            --out_hh $(@D)/{stem}.proto.hh \
            --out_test_cc $(@D)/{stem}_test.proto.hh
        """.format(proto = decl, stem = stem).strip(),
        visibility = visibility,
    )

    native.cc_library(
        name = name,
        srcs = [out_cc],
        hdrs = [out_hh, decl],
        deps = deps + ["//cs/net/json:object", "//cs/net/json:parsers", "//cs/net/json:serialize", "//cs/net/proto"],
        visibility = visibility,
    )

    native.cc_test(
        name = name + "_test",
        srcs = [out_test_cc],
        deps = [
            (":" + name),
            "@googletest//:gtest_main",
        ],
        visibility = visibility,
    )
