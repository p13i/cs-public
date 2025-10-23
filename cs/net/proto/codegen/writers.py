import os
from os import makedirs
from os.path import dirname, basename, join
from typing import List, Dict

from cs.net.proto.codegen.codegen_types import Proto
from cs.net.proto.codegen.helpers import cc_namespace, strip_bazel_out_paths
from cs.net.proto.codegen.constants import NEWLINE, BACKSLASH
from cs.net.proto.codegen.generators import (
    GenerateBuilderDeclaration,
    GenerateProtoImplClassDeclaration,
    GeneratedDeclarations,
    GenerateProtoImplClassDefinition,
    GenerateBuilderDefinition,
    GeneratedDefinitions,
    GenerateMatchersAndProtoTests,
)


def header_define(filename: str) -> str:
    """Return a sanitized header guard name for a given proto.hh file."""
    guard_path = join(dirname(filename), "gencode", basename(filename))
    return guard_path.replace(".", "_").replace("/", "_").upper()


def WriteGeneratedHhFile(
    PROTOS: Dict[str, Proto], gen_dir: str, input_filename: str
) -> None:
    """Generate the .hh header file for C++ protos."""
    previous_declarations: set[str] = set()
    ns = cc_namespace(
        strip_bazel_out_paths(join(dirname(gen_dir), basename(input_filename))),
        gen=True,
    )
    header_include = ns.replace("::", os.path.sep) + ".proto.hh"
    header_guard = header_define(input_filename)
    output_path = join(gen_dir, basename(input_filename))

    with open(output_path, "w+", encoding="utf-8") as f:
        f.write(
            f"""#ifndef {header_guard}
#define {header_guard}

#include "cs/net/json/object.hh"
#include "cs/result.hh"

#include "{strip_bazel_out_paths(header_include.replace('gencode' + os.path.sep, ''))}"
#include "{header_include}"

namespace {ns} {{

{NEWLINE.join([
    GeneratedDeclarations(struct, PROTOS, previous_declarations)
    for _, struct in PROTOS.items()
])}

{NEWLINE.join([
    GenerateProtoImplClassDeclaration(struct)
    for _, struct in PROTOS.items()
])}

{NEWLINE.join([
    GenerateBuilderDeclaration(struct, PROTOS)
    for _, struct in PROTOS.items()
])}

}} // namespace {ns}

#endif  // {header_guard}
"""
        )


def WriteGeneratedCcFile(
    PROTOS: Dict[str, Proto], gen_dir: str, input_filename: str
) -> None:
    """Generate the .cc implementation file for C++ protos."""
    makedirs(gen_dir, exist_ok=True)
    ns = cc_namespace(
        strip_bazel_out_paths(join(dirname(gen_dir), input_filename)), gen=True
    )
    path = join(gen_dir, basename(input_filename.replace(".proto.hh", ".proto.cc")))
    decl_include = strip_bazel_out_paths(
        join(dirname(gen_dir), basename(input_filename))
    )
    impl_decl_include = strip_bazel_out_paths(join(gen_dir, basename(input_filename)))
    previous_definitions: set[str] = set()

    with open(path, "w+", encoding="utf-8") as f:
        f.write(
            f"""#include "{decl_include}"
#include "{impl_decl_include}"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/json/serialize.hh"
#include "cs/result.hh"

#include <string>

namespace {ns} {{

namespace {{
using ::cs::net::json::Object;
using ::cs::net::json::SerializeObject;
using ::cs::InvalidArgument;
using ::cs::net::json::parsers::ParseNumber;
using ::cs::net::json::parsers::ParseObject;
using ::cs::net::json::parsers::ParseString;
using ::cs::net::json::parsers::ParseArray;
using ::cs::ResultOr;
}}

{NEWLINE.join([
    GeneratedDefinitions(proto, PROTOS, previous_definitions)
    for _, proto in PROTOS.items()
])}

}} // namespace {ns}

{NEWLINE.join([
    GenerateProtoImplClassDefinition(struct)
    for _, struct in PROTOS.items()
])}

{NEWLINE.join([
    GenerateBuilderDefinition(struct, PROTOS)
    for _, struct in PROTOS.items()
])}
"""
        )


def WriteGeneratedTestFile(
    PROTOS: Dict[str, Proto], gen_dir: str, input_filename: str
) -> None:
    """Generate a gtest/gmock test file for the given proto definitions."""
    test_abs_path = join(
        gen_dir, basename(input_filename.replace(".proto.hh", ".proto_test.cc"))
    )
    generated_header = join(gen_dir, basename(input_filename))
    previous_definitions: set[str] = set()

    def GenerateUsings(proto: Proto) -> str:
        fqn_proto = f"::{proto.namespace}::{cc_namespace(proto.filename, gen=False)}"
        if (
            fqn_proto == "::::"
            or not proto.namespace
            or not cc_namespace(proto.filename, gen=False)
        ):
            return ""
        return f"using {fqn_proto};\nusing {fqn_proto}FromString;"

    with open(test_abs_path, "w+", encoding="utf-8") as f:
        f.write(
            f"""
#include "{input_filename}"
#include "{generated_header}"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {{
{NEWLINE.join([
    GenerateUsings(proto)
    for _, proto in PROTOS.items() if GenerateUsings(proto)
])}
}}
using namespace testing;

template <typename InnerMatcher>
class AllElementsAreWithIndexMatcher {{
public:
    explicit AllElementsAreWithIndexMatcher(InnerMatcher inner) : inner_(inner) {{}}

    template <typename Range>
    bool MatchAndExplain(const Range& range, MatchResultListener* listener) const {{
        size_t index = 0;
        for (const auto& elem : range) {{
            if (!inner_(index, elem)) {{
                *listener << "element at index " << index << " does not match; value = " << elem;
                return false;
            }}
            ++index;
        }}
        return true;
    }}

    void DescribeTo(::std::ostream* os) const {{ *os << "each element satisfies inner matcher with index"; }}
    void DescribeNegationTo(::std::ostream* os) const {{ *os << "some element fails inner matcher with index"; }}

private:
    InnerMatcher inner_;
}};

template <typename InnerMatcher>
inline PolymorphicMatcher<AllElementsAreWithIndexMatcher<InnerMatcher>>
AllElementsAreWithIndex(InnerMatcher inner) {{
    return MakePolymorphicMatcher(AllElementsAreWithIndexMatcher<InnerMatcher>(inner));
}}

class Generated : public ::testing::Test {{}};

{NEWLINE.join([
    GenerateMatchersAndProtoTests(proto, PROTOS, previous_definitions)
    for _, proto in PROTOS.items()
])}
"""
        )


def WriteGeneratedBazelRules(repofiles: List[str], gen_dir: str) -> None:
    """Generate a BUILD file containing cc_library and cc_test rules for protos."""

    def build_rule(filename: str) -> str:
        if filename.startswith("/"):
            filename = filename[1:]

        base_name = basename(filename)
        lib_name = basename(
            filename.replace(".proto.hh", ".proto").replace(f"{BACKSLASH}.", "_")
        )
        test_name = basename(filename.replace(".proto.hh", ".proto_test"))

        return f"""cc_library(
    name = "{lib_name}",
    hdrs = ["{base_name}"],
    srcs = ["{basename(filename.replace('.proto.hh', '.proto.cc'))}"],
    visibility = ["//visibility:public"],
    deps = [
        "//{dirname(filename)}:{basename(filename.replace('.proto.hh', '.proto'))}",
        "//cs/net/json:object",
        "//cs/net/json:serialize",
        "//cs/net/json:parsers",
        "//cs:result",
    ],
)

cc_test(
    name = "{test_name}",
    srcs = ["{basename(filename.replace('.proto.hh', '.proto_test.cc'))}"],
    deps = [
        "//{dirname(filename)}/gencode:{lib_name}",
        "@googletest//:gtest_main",
    ],
)

"""

    build_content = NEWLINE.join(build_rule(filename) for filename in repofiles)

    with open(join(gen_dir, "BUILD"), "w", encoding="utf-8") as f:
        f.write(
            f"""# Auto-generated BUILD file

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])


{build_content}
"""
        )
