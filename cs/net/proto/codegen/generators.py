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


def GenerateProtoImplClassDefinition(proto: Proto):
    return f"""
// Proto definition of {proto.name}.
std::string {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}Impl::Serialize(unsigned int indent) const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}ToString(*that, indent);
}};
template<>
std::string cs::net::proto::Proto<{proto.namespace}::{proto.name}>::Serialize(unsigned int indent) const {{
    auto that = static_cast<const {proto.namespace}::{proto.name}*>(this);
    return {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}ToString(*that, indent);
}};
cs::ResultOr<{proto.namespace}::{proto.name}> {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}Impl::Parse(std::string s) {{
    return {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}FromString(s);
}}
template<>
cs::ResultOr<{proto.namespace}::{proto.name}> cs::net::proto::Proto<{proto.namespace}::{proto.name}>::Parse(std::string s) {{
    return {proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}FromString(s);
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
    else:
        raise ValueError(f"Unsupported type {T} for ToObject definition.")


def GenerateGetFieldPathSpecialization(proto: Proto, PROTOS: ProtoDB) -> str:
    """Generate GetFieldPath template specialization for a proto type."""
    proto_fqn = f"{proto.namespace}::{proto.name}"

    # Generate field path constants
    constants = []
    for field in proto.fields:
        const_name = f"k{proto.name}_{field.name}_path"
        constants.append(f'  inline const std::string {const_name} = "{field.name}";')

    constants_code = "\n".join(constants) if constants else ""

    # Generate template specialization
    cases = []
    for field in proto.fields:
        const_name = (
            f"cs::net::proto::db::codegen_helpers::k{proto.name}_{field.name}_path"
        )
        cases.append(
            f"      if (member_ptr == &{proto_fqn}::{field.name}) return {const_name};"
        )

    cases_code = "\n".join(cases) if cases else '      return "";'

    if not constants_code:
        return ""

    return f"""
#include "cs/net/proto/db/field_path_builder.gpt.hh"

namespace cs::net::proto::db::codegen_helpers {{
{constants_code}
}}

namespace cs::net::proto::db {{
  template<typename FieldType>
  std::string GetFieldPath(FieldType {proto_fqn}::*member_ptr) {{
{cases_code}
    return "";
  }}
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
