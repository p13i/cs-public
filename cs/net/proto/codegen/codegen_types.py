from dataclasses import dataclass
from typing import List, Dict


class Types:
    STRING = "std::string"
    FLOAT = "float"
    INT = "int"
    BOOL = "bool"
    VECTOR = "std::vector"
    MAP = "std::map"


@dataclass
class Field:
    name: str
    type: str


@dataclass
class Proto:
    name: str
    fields: List[Field]
    namespace: str
    filename: str


ProtoDB = Dict[str, Proto]
