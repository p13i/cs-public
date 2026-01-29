#!/usr/bin/env python3
# cs/net/proto/codegen/validator.py
from cs.net.proto.codegen.codegen_types import Types


def ValidateProtos(PROTOS):
    if not PROTOS:
        raise ValueError("No PROTOS found in input files")
    valid_types = {
        Types.STRING,
        Types.FLOAT,
        Types.BOOL,
        Types.INT,
        Types.VECTOR,
        Types.MAP,
    } | set(PROTOS.keys())

    for struct_name, struct in PROTOS.items():
        for field in struct.fields:
            if field.type not in valid_types:
                return (
                    False,
                    f"Invalid type '{field.type}' for field {field.name} in struct '{struct_name}'",
                )

    return True, "All PROTOS are valid"
