#!/usr/bin/env python3
# cs/net/proto/codegen/codegen_types.py
from dataclasses import dataclass, field
from typing import List, Dict, Union


class Types:
    STRING = "std::string"
    FLOAT = "float"
    INT = "int"
    BOOL = "bool"
    VECTOR = "std::vector"
    MAP = "std::map"
    JSON_OBJECT = "cs::net::json::Object"


@dataclass
class Field:
    name: str
    type: str
    original_type: str
    validations: List["ValidationAttr"] = field(default_factory=list)


@dataclass
class ValidationAttr:
    name: str
    args: List[Union[str, "ValidationAttr"]]


@dataclass
class Proto:
    name: str
    fields: List[Field]
    namespace: str
    filename: str


ProtoDB = Dict[str, Proto]
