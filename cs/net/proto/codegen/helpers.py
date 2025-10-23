from typing import Union, List
import os
from os.path import dirname

from cs.net.proto.codegen.codegen_types import Types, ProtoDB, Field
import re


def IsProto(type_: Union[Types, str], PROTOS: ProtoDB) -> bool:
    return type_ in PROTOS


def extract_T(s: str, PROTOS: ProtoDB) -> Types:
    if s.startswith(Types.VECTOR) and s.endswith(">"):
        # Extract T in std::vector<T>
        return s[len(Types.VECTOR + "<") : -1]
    raise ValueError(f"Expected a vector type, got: {s}")


def extract_Ts(s: str, PROTOS: ProtoDB) -> List[Types]:
    if not s:
        raise ValueError("Type cannot be empty or None")
    if s.startswith(Types.VECTOR):
        T = extract_T(s, PROTOS)
        return [s] + extract_Ts(T, PROTOS)
    elif s in (Types.STRING, Types.FLOAT, Types.INT, Types.BOOL):
        return [s]
    elif IsProto(s, PROTOS):
        return [s]
    else:
        raise ValueError(f"Unknown type in extract_Ts: {s}")


def strip_bazel_out_paths(path: str) -> str:
    if "bazel-out" in path:
        path = path[path.find("cs") :]
    return path


def cc_namespace(filename: str, gen=True) -> str:
    filename = strip_bazel_out_paths(filename)
    if not filename.endswith(".proto.hh"):
        raise ValueError(f"Expected filename to end with '.proto.hh', got: {filename}")
    ns = dirname(filename).replace("/", "::")
    if gen:
        ns += f"::gencode::{os.path.basename(filename)[:-9]}"
    if ns.startswith("::"):
        ns = ns[2:]
    return ns


def SerializeToKVMap(field: Field, kvmap_name: str, PROTOS: ProtoDB) -> str:
    What = None
    if field.type == Types.STRING:
        What = "String"
    elif field.type == Types.FLOAT:
        What = "Float"
    elif field.type == Types.INT:
        What = "Int"
    elif field.type == Types.BOOL:
        What = "Bool"
    elif field.type.startswith(Types.VECTOR):
        return f"""
        std::vector<Object> {field.name}_array;
        for (const auto& item : value.{field.name}) {{
            {field.name}_array.push_back({RecursiveDescribeT(extract_T(field.type, PROTOS), PROTOS)}ToObject(item));
        }}
        {kvmap_name}["{field.name}"] = Object::NewArray({field.name}_array);
        """
    if What:
        return f'{kvmap_name}["{field.name}"] = Object::New{What}(value.{field.name});'
    return f'{kvmap_name}["{field.name}"] = {RecursiveDescribeT(field.type, PROTOS)}ToObject(value.{field.name});'


def DefaultValue(type_: str, PROTOS: ProtoDB) -> str:
    if type_.startswith(Types.VECTOR):
        return "{}"
    elif IsProto(type_, PROTOS):
        return f"{type_}{{}}"
    elif type_ == Types.BOOL:
        return "false"
    elif type_ == Types.FLOAT:
        return "0.0"
    elif type_ == Types.INT:
        return "0"
    elif type_ == Types.MAP:
        return "{}"
    elif type_ == Types.STRING:
        return '""'
    return "/* unknown default */"


def ParseFromObject(field: Field, obj_name: str, res_name: str, PROTOS: ProtoDB) -> str:
    s = f"// {field.type} {field.name};\n"
    s += f'if ({obj_name}.has_key("{field.name}")) {{\n'

    if field.type.startswith(Types.VECTOR):
        s += f'SET_OR_RET(std::vector<Object> {field.name}_array, {obj_name}.get("{field.name}", std::vector<Object>()));\n'
        s += f"for (const auto& item : {field.name}_array) {{\n"
        s += f"    SET_OR_RET(auto item_as_obj, {RecursiveDescribeT(extract_T(field.type, PROTOS), PROTOS)}FromObject(item));\n"
        s += f"    {res_name}.{field.name}.emplace_back(item_as_obj);\n}}"
    elif IsProto(field.type, PROTOS):
        s += f'SET_OR_RET(Object {field.name}_obj, {obj_name}.get("{field.name}"));\n'
        s += f"SET_OR_RET({res_name}.{field.name}, {field.type}FromObject({field.name}_obj));"
    else:
        s += f'SET_OR_RET({res_name}.{field.name}, {obj_name}.get("{field.name}", {field.type}()));'

    s += f"\n}} else {{\n    {res_name}.{field.name} = {DefaultValue(field.type, PROTOS)};\n}};"
    return s


def FullyQualifiedType(T: Union[Types, str], PROTOS: ProtoDB) -> str:
    if T in PROTOS:
        return f"{PROTOS[T].namespace}::{T}"
    return T


def RecursiveDescribeT(T: Types, PROTOS: ProtoDB) -> str:
    if T.startswith(Types.VECTOR):
        return "VectorOf" + RecursiveDescribeT(extract_T(T, PROTOS), PROTOS)
    elif T == Types.STRING:
        return "String"
    elif T == Types.FLOAT:
        return "Float"
    elif T == Types.INT:
        return "Int"
    elif T == Types.BOOL:
        return "Bool"
    elif IsProto(T, PROTOS):
        return f"{T}"
    else:
        raise ValueError(f"Unknown type: {T}")
