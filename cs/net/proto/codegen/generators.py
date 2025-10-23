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
    if (!obj.is_map()) {{
        return TRACE(InvalidArgument("Object for {proto.name} is not a map."));
    }}
    {proto.namespace}::{proto.name} res;
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
    if (!obj.is_array()) {{
        return TRACE(InvalidArgument("Object for {T} is not an array."));
    }}
    {T} results;
    for (const auto& item : obj.as_array()) {{
        SET_OR_RET(auto item_as_obj, {RecursiveDescribeT(extract_T(T, PROTOS), PROTOS)}FromObject(item));
        results.emplace_back(item_as_obj);
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

    PREVIOUS_DEFINITIONS.update(vector_matchers)

    return f"""

{NEWLINE.join(vector_matchers)}

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
