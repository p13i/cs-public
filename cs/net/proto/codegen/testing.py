#!/usr/bin/env python3
# cs/net/proto/codegen/testing.py
from typing import Union
import json

from cs.net.proto.codegen.codegen_types import Types, ProtoDB, Proto, Field
from cs.net.proto.codegen.constants import NEWLINE, VECTOR_NUM_ELEMS
from cs.net.proto.codegen.helpers import (
    FullyQualifiedType,
    IsProto,
    cc_namespace,
    extract_T,
    extract_Ts,
    RecursiveDescribeT,
)


def ValidJsonValue(t: Union[Types], PROTOS: ProtoDB) -> Union[str, int, float]:
    if not t:
        raise ValueError("Type cannot be None or empty")

    if t == Types.STRING:
        return '"some-string"'
    elif t == Types.FLOAT:
        return "1.0"
    elif t == Types.INT:
        return "2"
    elif t == Types.BOOL:
        return "true"
    elif t.startswith(Types.VECTOR):
        T = extract_T(t, PROTOS)
        elem_value = ValidJsonValue(T, PROTOS)
        assert isinstance(elem_value, str), f"Expected string, got {type(elem_value)}"
        arr_str = json.dumps([json.loads(elem_value)] * VECTOR_NUM_ELEMS, indent=4)
        return arr_str
    elif t.startswith(Types.MAP):
        V = extract_T(t, PROTOS)
        value_json = ValidJsonValue(V, PROTOS)
        assert isinstance(value_json, str), f"Expected string, got {type(value_json)}"
        map_obj = {"key1": json.loads(value_json)}
        return json.dumps(map_obj, indent=4)
    elif t == Types.JSON_OBJECT:
        return "null"
    elif t in PROTOS:
        proto = PROTOS[t]
        obj = {
            field.name: json.loads(ValidJsonValue(field.type, PROTOS))
            for field in proto.fields
        }
        return json.dumps(obj, indent=4)
    else:
        raise ValueError(f"Invalid type '{t}' for proto field.")


def ValidCcTestMatcherValue(t: Types, PROTOS: ProtoDB) -> str:
    if t == Types.STRING:
        return '"some-string"'
    elif t == Types.FLOAT:
        return "1.0"
    elif t == Types.INT:
        return "2"
    elif t == Types.BOOL:
        return "true"
    elif t.startswith(Types.VECTOR):
        elem_type = extract_T(t, PROTOS)
        elem_vals = [ValidCcTestMatcherValue(elem_type, PROTOS)] * VECTOR_NUM_ELEMS
        if elem_type in PROTOS:
            proto = PROTOS[elem_type]
            elem_type = proto.namespace + "::" + proto.name
        return f"std::vector<{elem_type}>{{{(',' + NEWLINE).join(elem_vals)}}}"
    elif t.startswith(Types.MAP):
        value_type = extract_T(t, PROTOS)
        value_val = ValidCcTestMatcherValue(value_type, PROTOS)
        value_type_fqn = FullyQualifiedType(value_type, PROTOS)
        return f'std::map<std::string, {value_type_fqn}>{{{{"key1", {value_val}}}}}'
    elif t == Types.JSON_OBJECT:
        return "cs::net::json::Object()"
    elif IsProto(t, PROTOS):
        proto = PROTOS[t]
        builder = f"{proto.namespace + '::' + cc_namespace(proto.filename, gen=True)}::{proto.name}BuilderImpl()"
        for field in proto.fields:
            builder += (
                f".set_{field.name}({ValidCcTestMatcherValue(field.type, PROTOS)})"
            )
        return f"{builder}.Build()"
    else:
        raise ValueError(f"Invalid type '{t}' for proto field.")


def VectorMatcher(field: Field, proto: Proto, PROTOS: ProtoDB) -> str:
    if not field.type.startswith(Types.VECTOR):
        raise ValueError(
            f"VectorMatcher called with non-vector field type: {field.type}"
        )

    elem_type = extract_T(field.type, PROTOS)
    field_type = field.type
    if elem_type in PROTOS:
        proto = PROTOS[elem_type]
        fqn_proto = f"::{proto.namespace}::{cc_namespace(proto.filename, gen=False)}"
        field_type = f"{fqn_proto}{proto.name}"
        field_type = f"std::vector<{field_type}>"

    return (
        f"auto {RecursiveDescribeT(elem_type, PROTOS)}sVectorEq(const {field_type}& expected) {{\n"
        f"    return ::testing::AllOf(::testing::SizeIs(expected.size()));\n"
        f"}}\n"
    )


def MapMatcher(field: Field, proto: Proto, PROTOS: ProtoDB) -> str:
    if not field.type.startswith(Types.MAP):
        raise ValueError(f"MapMatcher called with non-map field type: {field.type}")

    value_type = extract_T(field.type, PROTOS)
    field_type_fqn = FullyQualifiedType(field.type, PROTOS)

    return (
        f"auto {RecursiveDescribeT(value_type, PROTOS)}sMapEq(const {field_type_fqn}& expected) {{\n"
        f"    return ::testing::AllOf(::testing::SizeIs(expected.size()));\n"
        f"}}\n"
    )


def TestMatcher(field: Field, PROTOS: ProtoDB) -> str:
    if field.type == Types.STRING:
        return "StrEq"
    elif field.type == Types.FLOAT:
        return "FloatEq"
    elif field.type in (Types.INT, Types.BOOL):
        return "Eq"
    elif field.type.startswith(Types.VECTOR):
        elem_type = extract_T(field.type, PROTOS)
        return f"{RecursiveDescribeT(elem_type, PROTOS)}sVectorEq"
    elif field.type.startswith(Types.MAP):
        value_type = extract_T(field.type, PROTOS)
        return f"{RecursiveDescribeT(value_type, PROTOS)}sMapEq"
    elif field.type == Types.JSON_OBJECT:
        return "ObjectStrEq"
    elif IsProto(field.type, PROTOS):
        return f"{field.type}Eq"
    else:
        raise ValueError(f"Invalid type '{field.type}' for proto field.")


def Asserts(field: Field, PROTOS: ProtoDB) -> str:
    return f"    ASSERT_THAT(struct_.{field.name}, {TestMatcher(field, PROTOS)}({ValidCcTestMatcherValue(field.type, PROTOS)}));"
