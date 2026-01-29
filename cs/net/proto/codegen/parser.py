#!/usr/bin/env python3
# cs/net/proto/codegen/parser.py
import re
from cs.net.proto.codegen.codegen_types import (
    Proto,
    Field,
    ProtoDB,
    Types,
    ValidationAttr,
)


def _split_top_level_commas(s: str) -> list[str]:
    parts = []
    buf = []
    depth = 0
    for ch in s:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth = max(0, depth - 1)
        if ch == "," and depth == 0:
            parts.append("".join(buf).strip())
            buf = []
        else:
            buf.append(ch)
    if buf:
        parts.append("".join(buf).strip())
    return [p for p in parts if p]


def _parse_attr(token: str) -> ValidationAttr:
    token = token.strip()
    if token.startswith("::validate::"):
        token = token[len("::validate::") :]
    elif token.startswith("validate::"):
        token = token[len("validate::") :]
    name = token
    args: list = []
    if "(" in token and token.endswith(")"):
        name, arg_str = token.split("(", 1)
        arg_str = arg_str[:-1]  # remove trailing )
        raw_args = _split_top_level_commas(arg_str)
        for a in raw_args:
            # Recursively parse nested validators (e.g., each(len_lt(32)))
            if "(" in a:
                args.append(_parse_attr(a))
            else:
                args.append(a.strip())
    return ValidationAttr(name=name.strip(), args=args)


def ParseInputHeader(input_string: str, filename: str) -> ProtoDB:
    """
    Parses a C++ header file containing DECLARE_PROTO structs
    and returns a ProtoDB mapping struct names to Proto objects.
    """
    # Allow namespaces with underscores, digits, and nested qualifiers.
    ns_matches = re.findall(r"namespace\s+([a-zA-Z0-9_:]+)", input_string)
    if not ns_matches:
        raise ValueError(f"No namespace found in file {filename}")
    namespace = ns_matches[0]

    PROTOS: ProtoDB = {}
    struct_pattern = re.findall(
        r"DECLARE_PROTO\((\w+)\)[\s]*\{([^}]*)\};", input_string
    )

    for struct_name, body in struct_pattern:
        # Parse fields manually to handle nested angle brackets
        # Remove comments first
        body_no_comments = re.sub(r"//.*?$", "", body, flags=re.MULTILINE)
        # Split by semicolons, but need to handle nested brackets
        # Use a simpler approach: find all field declarations
        # Pattern: type field_name;
        # But type can have nested brackets, so we need to match from the end
        matches = []
        # Split by semicolon, but be careful about nested brackets
        parts = re.split(r";(?![^<>]*>)", body_no_comments)
        for part in parts:
            part = part.strip()
            if not part or part.startswith("//"):
                continue
            # Find the last word (field name) before semicolon
            # Everything before that is the type
            name_match = re.search(r"(\w+)\s*$", part)
            if name_match:
                field_name = name_match.group(1)
                type_part = part[: name_match.start()].strip()
                if type_part:  # Only add if we have a type
                    matches.append((type_part, field_name))
        fields = []

        for field_type, field_name in matches:
            attrs: list[ValidationAttr] = []
            field_type = field_type.strip()
            # Extract validation attributes of the form [[validate::...]]
            while field_type.startswith("[["):
                end_idx = field_type.find("]]")
                if end_idx == -1:
                    break
                attr_block = field_type[2:end_idx]
                tokens = _split_top_level_commas(attr_block)
                attrs.extend(_parse_attr(tok) for tok in tokens if tok)
                field_type = field_type[end_idx + 2 :].strip()

            # Capture original type before normalization
            original_type = field_type
            # Normalize field types
            if field_type.endswith("*"):
                field_type = field_type[:-1]
            elif field_type in ("string", "std::string"):
                field_type = Types.STRING
            elif field_type.startswith("std::vector") or field_type.startswith(
                "vector"
            ):
                # Keep the full type including template parameters
                pass
            elif field_type.startswith("std::map") or field_type.startswith("map"):
                # Keep the full type including template parameters
                pass

            fields.append(
                Field(
                    name=field_name,
                    type=field_type,
                    original_type=original_type,
                    validations=attrs,
                )
            )

        PROTOS[struct_name] = Proto(
            name=struct_name,
            fields=fields,
            namespace=namespace,
            filename=filename,
        )

    return PROTOS
