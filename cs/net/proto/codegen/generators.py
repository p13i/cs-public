#!/usr/bin/env python3
# cs/net/proto/codegen/generators.py
from typing import Union
from cs.net.proto.codegen.codegen_types import Proto, Types, ProtoDB, Field
from cs.net.proto.codegen.constants import NEWLINE
from cs.net.proto.codegen.helpers import (
    IsProto,
    FullyQualifiedType,
    ParseFromObject,
    SerializeToKVMap,
    cc_namespace,
    extract_T,
    extract_Ts,
    RecursiveDescribeT,
)
from cs.net.proto.codegen.testing import (
    ValidJsonValue,
    ValidCcTestMatcherValue,
    VectorMatcher,
    MapMatcher,
    TestMatcher,
    Asserts,
)


def GenerateBuilderDefinition(proto: Proto, PROTOS: ProtoDB):
    return f"""// Builder definition for {proto.name}.
{proto.namespace}::{proto.name} {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}BuilderImpl::Build() {{
    return instance_;
}}

template<>
cs::net::proto::ProtoBuilder<{proto.namespace}::{proto.name}> cs::net::proto::Proto<{proto.namespace}::{proto.name}>::Builder() {{
    return {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}BuilderImpl{{}};
}};

template<>
{proto.namespace}::{proto.name} cs::net::proto::ProtoBuilder<{proto.namespace}::{proto.name}>::Build() {{
    return this->instance_;
}};

{NEWLINE.join([
    f"::{proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}BuilderImpl& ::{proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}BuilderImpl::set_{field.name}(const {FullyQualifiedType(field.type, PROTOS)}& value) {{this->instance_.{field.name} = value; return *this;}}"
    for field in proto.fields
])}
"""


def GenerateProtoImplClassDeclaration(proto: Proto):
    return f"""
// Proto declaration for implementation of {proto.name}.
class {proto.name}Impl : public {proto.namespace}::{proto.name} {{
    public:
    std::string Serialize(unsigned int indent = 0) const;
    cs::ResultOr<{proto.name}> Parse(std::string s);
    cs::net::json::Object ToObject() const override;
    cs::ResultOr<{proto.name}> FromObject(cs::net::json::Object obj) const override;
}};
"""


def GenerateMetaImplementation(proto: Proto, PROTOS: ProtoDB):
    proto_fqn = f"{proto.namespace}::{proto.name}"
    meta_ns = "cs::net::proto::protos"
    meta_gen_ns = f"{meta_ns}::gencode::meta"

    # Build field metas - create vector and push_back each field
    if proto.fields:
        field_builds = []
        for field in proto.fields:
            # Escape quotes and backslashes in original_type for string literals
            type_str = field.original_type.replace("\\", "\\\\").replace('"', '\\"')
            field_build = f"""fields.push_back(::{meta_gen_ns}::ProtoFieldMetaBuilderImpl{{}}
        .set_name("{field.name}")
        .set_type("{type_str}")
        .Build());"""
            field_builds.append(field_build)
        fields_code = "\n    ".join(field_builds)
        fields_setup = f"""std::vector<{meta_ns}::ProtoFieldMeta> fields;
    {fields_code}"""
    else:
        fields_setup = f"std::vector<{meta_ns}::ProtoFieldMeta> fields;"

    return f"""template<>
{meta_ns}::ProtoMeta cs::net::proto::Proto<{proto_fqn}>::Meta() const {{
    {fields_setup}
    return ::{meta_gen_ns}::ProtoMetaBuilderImpl{{}}
        .set_name("{proto_fqn}")
        .set_fields(fields)
        .Build();
}}
"""


def GenerateOperatorEqualsDeclaration(proto: Proto) -> str:
    """Generate operator== declaration in the proto namespace."""
    return f"""namespace {proto.namespace} {{
bool operator==(const {proto.name}& a, const {proto.name}& b);
}}  // namespace {proto.namespace}
"""


def GenerateOperatorEqualsDefinition(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate operator== definition comparing all fields."""
    if not proto.fields:
        return f"""namespace {proto.namespace} {{
bool operator==(const {proto.name}& a, const {proto.name}& b) {{
  return true;
}}
}}  // namespace {proto.namespace}
"""

    def eq_part(f: Field) -> str:
        if f.type == Types.JSON_OBJECT:
            return f"a.{f.name}.str() == b.{f.name}.str()"
        return f"a.{f.name} == b.{f.name}"

    parts = [eq_part(f) for f in proto.fields]
    body = " && ".join(parts)
    return f"""namespace {proto.namespace} {{
bool operator==(const {proto.name}& a, const {proto.name}& b) {{
  return {body};
}}
}}  // namespace {proto.namespace}
"""


def GenerateProtoImplClassDefinition(proto: Proto):
    gen_ns = proto.namespace + "::" + cc_namespace(proto.filename, gen=True)
    return f"""
// Proto definition of {proto.name}.
std::string {gen_ns}::{proto.name}Impl::Serialize(unsigned int indent) const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {gen_ns}::{proto.name}ToString(*that, indent);
}};
template<>
std::string cs::net::proto::Proto<{proto.namespace}::{proto.name}>::Serialize(unsigned int indent) const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {gen_ns}::{proto.name}ToString(*that, indent);
}};
cs::ResultOr<{proto.namespace}::{proto.name}> {gen_ns}::{proto.name}Impl::Parse(std::string s) {{
    return {gen_ns}::{proto.name}FromString(s);
}}
template<>
cs::ResultOr<{proto.namespace}::{proto.name}> cs::net::proto::Proto<{proto.namespace}::{proto.name}>::Parse(std::string s) {{
    return {gen_ns}::{proto.name}FromString(s);
}}
cs::net::json::Object {gen_ns}::{proto.name}Impl::ToObject() const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {gen_ns}::{proto.name}ToObject(*that);
}};
template<>
cs::net::json::Object cs::net::proto::Proto<{proto.namespace}::{proto.name}>::ToObject() const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {gen_ns}::{proto.name}ToObject(*that);
}};
cs::ResultOr<{proto.namespace}::{proto.name}> {gen_ns}::{proto.name}Impl::FromObject(cs::net::json::Object obj) const {{
    return {gen_ns}::{proto.name}FromObject(obj);
}}
template<>
cs::ResultOr<{proto.namespace}::{proto.name}> cs::net::proto::Proto<{proto.namespace}::{proto.name}>::FromObject(cs::net::json::Object obj) const {{
    return {gen_ns}::{proto.name}FromObject(obj);
}}
"""


def flatten2d(lst):
    """Flattens a 2D list into a 1D list."""
    return [item for sublist in lst for item in sublist]


def GeneratedDeclarations(
    proto: Proto, PROTOS: ProtoDB, PREVIOUS_DECLARATIONS: set[Union[Types, str]]
):
    unique_Ts = (
        set(T for field in proto.fields for T in extract_Ts(field.type, PROTOS))
        - PREVIOUS_DECLARATIONS
    )
    unique_Ts.update((Types.STRING, Types.INT, Types.FLOAT, Types.BOOL))
    unique_Ts.add(proto.name)
    to_object_decls = [
        GenerateToObjectDeclaration(T, PROTOS)
        for T in unique_Ts
        if GenerateToObjectDeclaration(T, PROTOS) not in PREVIOUS_DECLARATIONS
    ]
    from_object_decls = [
        GenerateFromObjectDeclaration(T, PROTOS)
        for T in unique_Ts
        if GenerateFromObjectDeclaration(T, PROTOS) not in PREVIOUS_DECLARATIONS
    ]
    PREVIOUS_DECLARATIONS.update(to_object_decls)
    PREVIOUS_DECLARATIONS.update(from_object_decls)
    return f"""// Function declarations for {proto.name}.
cs::ResultOr<{proto.namespace}::{proto.name}> {proto.name}FromString(std::string str);
std::string {proto.name}ToString({proto.name} struct_, unsigned int indent = 0);
{NEWLINE.join(to_object_decls)}
{NEWLINE.join(from_object_decls)}
"""


def GeneratedDefinitions(
    proto: Proto, PROTOS: ProtoDB, PREVIOUS_DEFINITIONS: set[Union[Types, str]]
):
    unique_Ts = set(T for field in proto.fields for T in extract_Ts(field.type, PROTOS))
    unique_Ts.update((Types.STRING, Types.INT, Types.FLOAT, Types.BOOL))
    unique_Ts.add(proto.name)
    to_object_defs = [
        GenerateToObjectDefinition(T, PROTOS)
        for T in unique_Ts
        if GenerateToObjectDefinition(T, PROTOS) not in PREVIOUS_DEFINITIONS
    ]
    PREVIOUS_DEFINITIONS.update(to_object_defs)
    return f"""// Function definitions for {proto.name}.
ResultOr<{proto.namespace}::{proto.name}> {proto.name}FromString(std::string str) {{
    SET_OR_RET(Object obj, ParseObject(str));
    return {cc_namespace(proto.filename, gen=True)}::{proto.name}FromObject(obj);
}};
std::string {proto.name}ToString({proto.name} value, unsigned int indent) {{
    Object object = {cc_namespace(proto.filename, gen=True)}::{proto.name}ToObject(value);
    return SerializeObject(object, indent);
}};
{NEWLINE.join(to_object_defs)}
"""


def GenerateToObjectDeclaration(
    T: Union[Types, str], PROTOS: ProtoDB, semicolon: bool = True
):
    return f"""
cs::net::json::Object {RecursiveDescribeT(T, PROTOS)}ToObject({T} value){";" if semicolon else ""}
"""


def GenerateFromObjectDeclaration(
    T: Union[Types, str], PROTOS: ProtoDB, semicolon: bool = True
):
    return f"""cs::ResultOr<{T}> {RecursiveDescribeT(T, PROTOS)}FromObject(cs::net::json::Object obj){";" if semicolon else ""}
"""


def GenerateToObjectDefinition(T: Union[Types, str], PROTOS: ProtoDB):
    if IsProto(T, PROTOS):
        proto = PROTOS[T]
        return f"""{GenerateToObjectDeclaration(T, PROTOS, semicolon=False)} {{
    cs::net::json::Object::KVMap kv;
    {NEWLINE.join([
        SerializeToKVMap(field, "kv", PROTOS) 
        for field in proto.fields])}
    return cs::net::json::Object(kv);
}};

cs::ResultOr<{proto.namespace}::{proto.name}> {proto.name}FromObject(cs::net::json::Object obj) {{
    if (!obj.is(std::map<std::string, cs::net::json::Object>())) {{
        return TRACE(InvalidArgument("Object for {proto.name} is not a map."));
    }}
    {proto.namespace}::{proto.name} res{{}};  // Value-initialize all fields (bool=false, int=0, string="")
    {NEWLINE.join([ParseFromObject(field, "obj", "res", PROTOS) for field in proto.fields])}
    return res;
}};
"""
    elif T.startswith(Types.VECTOR) and T.endswith(">"):
        return f"""{GenerateToObjectDeclaration(T, PROTOS, semicolon=False)} {{
    std::vector<cs::net::json::Object> objects;
    for (const auto& item : value) {{
        objects.push_back({RecursiveDescribeT(extract_T(T, PROTOS), PROTOS)}ToObject(item));
    }}
    return cs::net::json::Object::NewArray(objects);
}};

{GenerateFromObjectDeclaration(T, PROTOS, semicolon=False)} {{
    if (!obj.is(std::vector<cs::net::json::Object>())) {{
        return TRACE(InvalidArgument("Object for {T} is not an array."));
    }}
    {T} results;
    results.reserve(obj.as(std::vector<cs::net::json::Object>()).size());
    for (const auto& item : obj.as(std::vector<cs::net::json::Object>())) {{
        SET_OR_RET(auto item_as_obj, {RecursiveDescribeT(extract_T(T, PROTOS), PROTOS)}FromObject(item));
        results.push_back(item_as_obj);
    }}
    return results;
}};
"""
    elif T.startswith(Types.MAP) and T.endswith(">"):
        V = extract_T(T, PROTOS)
        return f"""{GenerateToObjectDeclaration(T, PROTOS, semicolon=False)} {{
    cs::net::json::Object::KVMap map_obj;
    for (const auto& [key, val] : value) {{
        map_obj[key] = {RecursiveDescribeT(V, PROTOS)}ToObject(val);
    }}
    return cs::net::json::Object(map_obj);
}};

{GenerateFromObjectDeclaration(T, PROTOS, semicolon=False)} {{
    if (!obj.is(std::map<std::string, cs::net::json::Object>())) {{
        return TRACE(InvalidArgument("Object for {T} is not a map."));
    }}
    {T} results;
    for (const auto& [key, val] : obj.as(std::map<std::string, cs::net::json::Object>())) {{
        SET_OR_RET(auto val_as_obj, {RecursiveDescribeT(V, PROTOS)}FromObject(val));
        results[key] = val_as_obj;
    }}
    return results;
}};
"""
    elif T in (Types.STRING, Types.INT, Types.FLOAT, Types.BOOL):
        return f"""{GenerateToObjectDeclaration(T, PROTOS, semicolon=False)} {{
    return cs::net::json::Object::New{RecursiveDescribeT(T, PROTOS)}(value);
}};

{GenerateFromObjectDeclaration(T, PROTOS, semicolon=False)} {{
    if (!obj.is({T}())) {{
        return TRACE(InvalidArgument("Object is not a {T}."));
    }}
    return obj.as({T}());
}};
"""
    elif T == Types.JSON_OBJECT:
        return f"""{GenerateToObjectDeclaration(T, PROTOS, semicolon=False)} {{
    return value;
}};

{GenerateFromObjectDeclaration(T, PROTOS, semicolon=False)} {{
    return obj;
}};
"""
    else:
        raise ValueError(f"Unsupported type {T} for ToObject definition.")


def GenerateGetFieldPathSpecialization(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate GetFieldPath full specializations for (proto, field_type).

    Emit full specializations of the default template so overload resolution
    chooses them over the default (abort) implementation.
    """
    proto_fqn = f"{proto.namespace}::{proto.name}"

    # Generate field path constants
    constants = []
    for field in proto.fields:
        const_name = f"k{proto.name}_{field.name}_path"
        constants.append(f'  inline const std::string {const_name} = "{field.name}";')

    constants_code = "\n".join(constants) if constants else ""

    if not constants_code:
        return ""

    # One full specialization per distinct field type (template <> GetFieldPath<T, FieldType>).
    seen_field_types = set()
    spec_blocks = []
    for field in proto.fields:
        field_fqn = FullyQualifiedType(field.type, PROTOS)
        if field_fqn in seen_field_types:
            continue
        seen_field_types.add(field_fqn)
        # Body: if (member_ptr == &Proto::field) return kpath; for each field of this type.
        cases = []
        for f in proto.fields:
            if FullyQualifiedType(f.type, PROTOS) != field_fqn:
                continue
            const_name = (
                f"cs::net::proto::db::codegen_helpers::k{proto.name}_{f.name}_path"
            )
            cases.append(
                f"  if (member_ptr == &{proto_fqn}::{f.name}) return {const_name};"
            )
        cases_code = "\n".join(cases) if cases else "  std::abort();"
        cases_code += f"""
  std::fprintf(stderr,
               "GetFieldPath: unknown member pointer for type %s\\n",
               "{proto_fqn}");
  std::abort();"""
        spec_blocks.append(
            f"""template <>
inline std::string GetFieldPath<{proto_fqn}, {field_fqn}>(
    {field_fqn} {proto_fqn}::*member_ptr) {{
{cases_code}
}}"""
        )

    specs_code = "\n\n".join(spec_blocks)

    return f"""
#include <cstdio>
#include <cstdlib>

#include "cs/net/proto/db/field_path_builder.gpt.hh"

namespace cs::net::proto::db::codegen_helpers {{
{constants_code}
}}

namespace cs::net::proto::db {{
{specs_code}
}}
"""


def GenerateFieldPathBuilderSupport(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate field path builder support for log >> request >> path syntax."""
    proto_fqn = f"{proto.namespace}::{proto.name}"

    # Generate field path builder instance and field constants
    builder_name = (
        proto.name[0].lower() + proto.name[1:] if proto.name else proto.name.lower()
    )

    field_constants = []
    for field in proto.fields:
        # Use prefixed names to avoid conflicts across protos
        field_constants.append(
            f"  inline constexpr auto {proto.name.lower()}_{field.name} = &{proto.name}::{field.name};"
        )

    if not field_constants:
        return ""

    constants_code = "\n".join(field_constants)

    # FieldPathBuilder contains std::string so cannot be constexpr
    # Generate an inline function instead
    return f"""
#include "cs/net/proto/db/field_path_builder.gpt.hh"

namespace {proto.namespace} {{
  inline cs::net::proto::db::FieldPathBuilder<{proto.name}> {builder_name}() {{
    return cs::net::proto::db::FieldPathBuilder<{proto.name}>{{}};
  }}
{constants_code}
}}  // namespace {proto.namespace}
"""


def GenerateGetFieldPathExplicitInstantiations(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate explicit template instantiations for GetFieldPath in .cc.

    With full specializations in the header, no .cc instantiations are needed.
    """
    return ""


def GenerateJsonProtoDefinitions(proto: Proto):
    return f"""std::string {cc_namespace(proto.filename, gen=True)}::{proto.name}::Serialize(unsigned int indent) {{
    return {cc_namespace(proto.filename, gen=True)}::{proto.name}ToString(this, indent);
}}
"""


def GenerateMatchersAndProtoTests(
    proto: Proto, PROTOS: ProtoDB, PREVIOUS_DEFINITIONS: set[str]
):
    vector_matchers: set[str] = (
        set(
            [
                VectorMatcher(field, proto, PROTOS)
                for field in proto.fields
                if field.type.startswith(Types.VECTOR)
            ]
        )
        - PREVIOUS_DEFINITIONS
    )
    map_matchers: set[str] = (
        set(
            [
                MapMatcher(field, proto, PROTOS)
                for field in proto.fields
                if field.type.startswith(Types.MAP)
            ]
        )
        - PREVIOUS_DEFINITIONS
    )

    PREVIOUS_DEFINITIONS.update(vector_matchers)
    PREVIOUS_DEFINITIONS.update(map_matchers)

    return f"""

{NEWLINE.join(vector_matchers)}
{NEWLINE.join(map_matchers)}

auto {proto.name}Eq(const {proto.namespace + '::' + proto.name}& expected) {{
   return ::testing::AllOf(
         {",".join([
            f"::testing::Field(&{proto.namespace + '::' + proto.name}::{field.name}, {TestMatcher(field, PROTOS)}(expected.{field.name}))"
            for field in proto.fields
         ] or ["Eq()"])}
     );
}}

class {proto.name}Test : public ::testing::Test {{}};

TEST_F({proto.name}Test, {proto.name}FromString) {{
    auto result = {proto.namespace + "::" + cc_namespace(proto.filename, gen=True)}::{proto.name}FromString(R"json(
{ValidJsonValue(proto.name, PROTOS)}
)json");
    ASSERT_THAT(result.ok(), IsTrue()) << result.message();
    {proto.namespace + cc_namespace(proto.filename, gen=False)}::{proto.name} struct_ = result.value();
    {NEWLINE.join([
        Asserts(field, PROTOS)
        for field in proto.fields
    ])}
}}
"""


def GenerateToObjectFromObjectTests(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate TEST_F cases for ToObject/FromObject round-trip."""
    gen_ns = proto.namespace + "::" + cc_namespace(proto.filename, gen=True)
    fqn = f"{proto.namespace}::{proto.name}"

    # Build a valid C++ value using the Builder
    builder_expr = ValidCcTestMatcherValue(proto.name, PROTOS)

    # For protos with fields, use the Eq matcher; for empty protos, just
    # assert the round-trip succeeded (no fields to compare).
    if proto.fields:
        value_assert = f"EXPECT_THAT(result.value(), {proto.name}Eq(original));"
    else:
        value_assert = "(void)result.value();  // No fields to compare."

    return f"""
class {proto.name}ToObjectFromObjectTest : public ::testing::Test {{}};

TEST_F({proto.name}ToObjectFromObjectTest, FreeFunctionRoundTrip) {{
    {fqn} original = {builder_expr};
    cs::net::json::Object obj = {gen_ns}::{proto.name}ToObject(original);
    auto result = {gen_ns}::{proto.name}FromObject(obj);
    ASSERT_THAT(result.ok(), IsTrue()) << result.message();
    {value_assert}
}}

TEST_F({proto.name}ToObjectFromObjectTest, ImplMethodRoundTrip) {{
    {gen_ns}::{proto.name}Impl impl;
    {fqn} original = {builder_expr};
    static_cast<{fqn}&>(impl) = original;
    cs::net::json::Object obj = impl.ToObject();
    auto result = impl.FromObject(obj);
    ASSERT_THAT(result.ok(), IsTrue()) << result.message();
    {value_assert}
}}

TEST_F({proto.name}ToObjectFromObjectTest, ProtoVirtualDispatchRoundTrip) {{
    {gen_ns}::{proto.name}Impl impl;
    {fqn} original = {builder_expr};
    static_cast<{fqn}&>(impl) = original;
    cs::net::proto::Proto<{fqn}>* p = &impl;
    cs::net::json::Object obj = p->ToObject();
    auto result = p->FromObject(obj);
    ASSERT_THAT(result.ok(), IsTrue()) << result.message();
    {value_assert}
}}
"""


def GenerateBuilderDeclaration(proto: Proto, PROTOS: ProtoDB):
    return f"""// Builder declaration for {proto.name}.
class {proto.name}BuilderImpl : public cs::net::proto::ProtoBuilder<{proto.name}> {{
public:
    {proto.namespace}::{proto.name} Build();
{NEWLINE.join([
    f"{proto.name}BuilderImpl& set_{field.name}(const {FullyQualifiedType(field.type, PROTOS)}& value);" for field in proto.fields
])}
}};
"""
