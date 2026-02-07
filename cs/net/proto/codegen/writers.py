#!/usr/bin/env python3
# cs/net/proto/codegen/writers.py
import os
from os import makedirs
from os.path import dirname, basename, join
from typing import List, Dict
from pathlib import Path

from cs.net.proto.codegen.codegen_types import Proto, ValidationAttr, Types
from cs.net.proto.codegen.helpers import cc_namespace, strip_bazel_out_paths
from cs.net.proto.codegen.constants import NEWLINE, BACKSLASH
from cs.net.proto.codegen.generators import (
    GenerateBuilderDeclaration,
    GenerateBuilderDefinition,
    GenerateFieldPathBuilderSupport,
    GenerateGetFieldPathExplicitInstantiations,
    GenerateGetFieldPathSpecialization,
    GenerateMetaImplementation,
    GenerateOperatorEqualsDeclaration,
    GenerateOperatorEqualsDefinition,
    GenerateProtoImplClassDeclaration,
    GenerateProtoImplClassDefinition,
    GeneratedDeclarations,
    GeneratedDefinitions,
    GenerateMatchersAndProtoTests,
    GenerateToObjectFromObjectTests,
)


def header_define(filename: str) -> str:
    """Return a sanitized header guard name for a given proto.hh file."""
    guard_path = join(dirname(filename), "gencode", basename(filename))
    return guard_path.replace(".", "_").replace("/", "_").upper()


def _output_include_for_input(
    input_filename: str, gen_dir: str, out_root_rel: str | None
) -> str:
    if out_root_rel:
        base = Path(out_root_rel)
        rel = Path(input_filename)
        return (base / rel.parent / rel.name).as_posix()
    return strip_bazel_out_paths(join(dirname(gen_dir), basename(input_filename)))


def _output_include_for_generated(
    input_filename: str, gen_dir: str, out_root_rel: str | None
) -> str:
    if out_root_rel:
        base = Path(out_root_rel)
        rel = Path(input_filename)
        return (base / rel.parent / "gencode" / rel.name).as_posix()
    return strip_bazel_out_paths(join(gen_dir, basename(input_filename)))


def WriteGeneratedHhFile(
    PROTOS: Dict[str, Proto],
    gen_dir: str,
    input_filename: str,
    out_root_rel: str | None = None,
) -> None:
    """Generate the .hh header file for C++ protos."""
    previous_declarations: set[str] = set()
    proto_sample = next(iter(PROTOS.values()))
    ns = f"{proto_sample.namespace}::{cc_namespace(proto_sample.filename, gen=True)}"
    decl_include = _output_include_for_input(input_filename, gen_dir, out_root_rel)
    gen_header_include = _output_include_for_generated(
        input_filename, gen_dir, out_root_rel
    )
    header_guard = header_define(input_filename)
    output_path = join(gen_dir, basename(input_filename))

    with open(output_path, "w+", encoding="utf-8") as f:
        f.write(
            f"""#ifndef {header_guard}
#define {header_guard}

#include "cs/net/json/object.hh"
#include "cs/result.hh"

#include "{decl_include}"
#include "{gen_header_include}"

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

{NEWLINE.join([
    GenerateGetFieldPathSpecialization(struct, PROTOS)
    for _, struct in PROTOS.items()
    if GenerateGetFieldPathSpecialization(struct, PROTOS)
])}

{NEWLINE.join([
    GenerateFieldPathBuilderSupport(struct, PROTOS)
    for _, struct in PROTOS.items()
    if GenerateFieldPathBuilderSupport(struct, PROTOS)
])}

{NEWLINE.join([
    GenerateOperatorEqualsDeclaration(struct)
    for _, struct in PROTOS.items()
])}

#endif  // {header_guard}
"""
        )


def WriteGeneratedCcFile(
    PROTOS: Dict[str, Proto],
    gen_dir: str,
    input_filename: str,
    out_root_rel: str | None = None,
) -> None:
    """Generate the .cc implementation file for C++ protos."""
    makedirs(gen_dir, exist_ok=True)
    proto_sample = next(iter(PROTOS.values()))
    ns = f"{proto_sample.namespace}::{cc_namespace(proto_sample.filename, gen=True)}"
    path = join(gen_dir, basename(input_filename.replace(".proto.hh", ".proto.cc")))
    decl_include = _output_include_for_input(input_filename, gen_dir, out_root_rel)
    impl_decl_include = _output_include_for_generated(
        input_filename, gen_dir, out_root_rel
    )
    previous_definitions: set[str] = set()

    # Only include meta.proto.hh if this is not meta.proto itself
    is_meta_proto = basename(input_filename) == "meta.proto.hh"
    meta_include = (
        ""
        if is_meta_proto
        else '#include "cs/net/proto/protos/gencode/meta.proto.hh"\n'
    )

    # Skip explicit instantiations for meta.proto (internal, complex types)
    explicit_inst = ""
    if not is_meta_proto:
        explicit_inst = NEWLINE.join(
            [
                GenerateGetFieldPathExplicitInstantiations(struct, PROTOS)
                for _, struct in PROTOS.items()
                if GenerateGetFieldPathExplicitInstantiations(struct, PROTOS)
            ]
        )
    explicit_inst_block = ""
    if explicit_inst:
        explicit_inst_block = f"""
#include "cs/net/proto/db/field_path_builder.gpt.hh"

{explicit_inst}
"""

    with open(path, "w+", encoding="utf-8") as f:
        f.write(
            f"""#include "{decl_include}"
#include "{impl_decl_include}"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/net/json/serialize.hh"
#include "cs/result.hh"
{meta_include}#include <string>

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

{NEWLINE.join([
    GenerateMetaImplementation(struct, PROTOS)
    for _, struct in PROTOS.items()
])}

{NEWLINE.join([
    GenerateOperatorEqualsDefinition(struct, PROTOS)
    for _, struct in PROTOS.items()
])}
{explicit_inst_block}
"""
        )


def _object_str_eq_helper(PROTOS: Dict[str, Proto]) -> str:
    """Emit ObjectStrEq matcher helper if any proto has a JSON_OBJECT field."""
    has_json_object = any(
        field.type == Types.JSON_OBJECT
        for proto in PROTOS.values()
        for field in proto.fields
    )
    if not has_json_object:
        return ""
    return """
inline auto ObjectStrEq(const cs::net::json::Object& exp) {
  return ::testing::ResultOf(
      [](const cs::net::json::Object& o) { return o.str(); },
      ::testing::StrEq(exp.str()));
}
"""


def WriteGeneratedTestFile(
    PROTOS: Dict[str, Proto],
    gen_dir: str,
    input_filename: str,
    out_root_rel: str | None = None,
) -> None:
    """Generate a gtest/gmock test file for the given proto definitions."""
    test_abs_path = join(
        gen_dir, basename(input_filename.replace(".proto.hh", ".proto_test.cc"))
    )
    decl_include = strip_bazel_out_paths(input_filename)
    generated_header = _output_include_for_generated(
        input_filename, gen_dir, out_root_rel
    )
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
#include "{decl_include}"
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

{_object_str_eq_helper(PROTOS)}

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

{NEWLINE.join([
    GenerateToObjectFromObjectTests(proto, PROTOS)
    for _, proto in PROTOS.items()
])}
"""
        )


def _field_path_const(proto: Proto, field) -> str:
    return f'inline constexpr char k{proto.name}_{field.name}_path[] = "{field.name}";'


def _render_validator(
    attr: ValidationAttr, proto: Proto, field, extra_consts: list = None
) -> str:
    ns = proto.namespace
    field_expr = f"::cs::net::proto::validation::field<&{ns}::{proto.name}::{field.name}, k{proto.name}_{field.name}_path>"

    if attr.name == "required":
        return f"::cs::net::proto::validation::required<{field_expr}>"
    if attr.name == "email":
        return f"::cs::net::proto::validation::email<{field_expr}>"
    if attr.name in ("gt", "ge", "lt", "le", "len_gt", "len_lt"):
        if not attr.args:
            raise ValueError(f"{attr.name} requires an argument")
        arg = attr.args[0]
        return f"::cs::net::proto::validation::{attr.name}<{field_expr}, {arg}>"
    if attr.name == "enum_in":
        if not attr.args:
            raise ValueError("enum_in requires at least one allowed value")
        enum_refs = ", ".join(
            [f"k{proto.name}_{field.name}_enum_{idx}" for idx in range(len(attr.args))]
        )
        return f"::cs::net::proto::validation::enum_in<{field_expr}, {enum_refs}>"
    if attr.name == "enum_in_ref":
        if not attr.args:
            raise ValueError("enum_in_ref requires at least one allowed value")
        enum_refs = ", ".join(attr.args)
        return f"::cs::net::proto::validation::enum_in<{field_expr}, {enum_refs}>"
    if attr.name == "matches":
        if not attr.args:
            raise ValueError("matches requires a pattern argument")
        pat_const = f"k{proto.name}_{field.name}_matches_{len(attr.args[0])}_pattern"
        return f"::cs::net::proto::validation::matches<{field_expr}, {pat_const}>"
    if attr.name == "iso8601":
        return f"::cs::net::proto::validation::iso8601<{field_expr}>"
    if attr.name == "uuid":
        return f"::cs::net::proto::validation::uuid<{field_expr}>"
    if attr.name == "base64":
        return f"::cs::net::proto::validation::base64<{field_expr}>"
    if attr.name == "filename":
        return f"::cs::net::proto::validation::filename<{field_expr}>"
    if attr.name == "host":
        return f"::cs::net::proto::validation::host<{field_expr}>"
    if attr.name == "port":
        return f"::cs::net::proto::validation::port<{field_expr}>"
    if attr.name == "oneof_set":
        return f"::cs::net::proto::validation::oneof_set<{field_expr}>"
    if attr.name == "custom":
        if not attr.args:
            raise ValueError("custom requires a token argument")
        token_const = f"k{proto.name}_{field.name}_custom_token"
        return f"::cs::net::proto::validation::custom<{field_expr}, {token_const}>"

    raise ValueError(f"Unsupported validation attribute '{attr.name}'")


def WriteGeneratedValidationFile(
    PROTOS: Dict[str, Proto],
    gen_dir: str,
    input_filename: str,
    out_root_rel: str | None = None,
) -> None:
    """Generate a .validate.hh file with validation Rules aliases."""
    if not PROTOS:
        return
    proto_sample = next(iter(PROTOS.values()))
    ns = f"{proto_sample.namespace}::{cc_namespace(proto_sample.filename, gen=True)}"
    decl_include = _output_include_for_input(input_filename, gen_dir, out_root_rel)
    header_guard = header_define(input_filename + ".validate")
    output_path = join(
        gen_dir, basename(input_filename).replace(".proto.hh", ".validate.hh")
    )

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(
            f"""#ifndef {header_guard}
#define {header_guard}

#include "cs/net/proto/validators.gpt.hh"
#include "{decl_include}"

namespace {ns} {{
namespace validation_generated {{
using namespace ::cs::net::proto::validation;

"""
        )
        # Field path constants
        for _, proto in PROTOS.items():
            for field in proto.fields:
                f.write(f"{_field_path_const(proto, field)};\n")
            f.write("\n")

        for _, proto in PROTOS.items():
            validators = []
            extra_consts = []
            for field in proto.fields:
                for attr in field.validations:
                    if attr.name == "matches" and attr.args:
                        pat_name = f"k{proto.name}_{field.name}_matches_{len(attr.args[0])}_pattern"
                        extra_consts.append(
                            f"inline constexpr char {pat_name}[] = {attr.args[0]};"
                        )
                    if attr.name == "custom" and attr.args:
                        tok_name = f"k{proto.name}_{field.name}_custom_token"
                        extra_consts.append(
                            f"inline constexpr char {tok_name}[] = {attr.args[0]};"
                        )
                    if attr.name == "enum_in" and attr.args:
                        for idx, arg in enumerate(attr.args):
                            enum_name = f"k{proto.name}_{field.name}_enum_{idx}"
                            extra_consts.append(
                                f"inline constexpr char {enum_name}[] = {arg};"
                            )
                    # enum_in_ref: no extra_consts; refs are external (e.g.
                    # ::cs::net::http::kContentType*).
                    validators.append(_render_validator(attr, proto, field))
            # emit extra consts (patterns/tokens)
            for line in extra_consts:
                f.write(line + "\n")
            if validators:
                rules_expr = ",\n    ".join(validators)
                f.write(f"using {proto.name}Rules = all<\n    {rules_expr}\n>;\n\n")
            else:
                f.write(f"using {proto.name}Rules = noop;\n\n")

        f.write(
            f"""}}
}}  // namespace {ns}

#endif  // {header_guard}
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
