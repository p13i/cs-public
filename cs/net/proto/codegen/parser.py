import re
from cs.net.proto.codegen.codegen_types import Proto, Field, ProtoDB, Types


def ParseInputHeader(input_string: str, filename: str) -> ProtoDB:
    """
    Parses a C++ header file containing DECLARE_PROTO structs
    and returns a ProtoDB mapping struct names to Proto objects.
    """
    ns_matches = re.findall(r"namespace ([a-z\:]+)", input_string)
    if not ns_matches:
        raise ValueError(f"No namespace found in file {filename}")
    namespace = ns_matches[0]

    PROTOS: ProtoDB = {}
    struct_pattern = re.findall(
        r"DECLARE_PROTO\((\w+)\)[\s]*\{([^}]*)\};", input_string
    )

    for struct_name, body in struct_pattern:
        matches = re.findall(r"\s*(\w[\w\<\:\>\*]*)\s+([_\w^;]+);", body)
        fields = []

        for field_type, field_name in matches:
            # Normalize field types
            if field_type.endswith("*"):
                field_type = field_type[:-1]
            elif field_type in ("string", "std::string"):
                field_type = Types.STRING
            elif field_type in ("vector", "std::vector"):
                field_type = Types.VECTOR

            fields.append(Field(name=field_name, type=field_type))

        PROTOS[struct_name] = Proto(
            name=struct_name,
            fields=fields,
            namespace=namespace,
            filename=filename,
        )

    return PROTOS
