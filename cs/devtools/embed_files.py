#!/usr/bin/env python3
# cs/devtools/embed_files.py
import argparse, os, re, textwrap


def norm_symbol(s: str) -> str:
    s = re.sub(r"[^A-Za-z0-9_]", "_", s)
    if re.match(r"^[0-9]", s):
        s = "_" + s
    return s


def bytes_literal(b: bytes) -> str:
    return ", ".join(f"0x{v:02x}" for v in b)


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--prefix", required=True)
    p.add_argument("--symbol-suffix", dest="symbol_suffix", required=True)
    p.add_argument("--out_cc", required=True)
    p.add_argument("--out_h", required=True)
    p.add_argument("--file", nargs=2, action="append", default=[])
    a = p.parse_args()

    suffix = norm_symbol(a.symbol_suffix)
    table_sym = f"kEmbeddedFiles_{suffix}"
    count_sym = f"kEmbeddedFilesCount_{suffix}"

    os.makedirs(os.path.dirname(a.out_cc), exist_ok=True)
    os.makedirs(os.path.dirname(a.out_h), exist_ok=True)

    entries = []
    for logical, path in a.file:
        with open(path, "rb") as f:
            data = f.read()
        sym = f"{a.prefix}_{norm_symbol(logical)}"
        entries.append((logical, sym, data))

    # Header
    with open(a.out_h, "w") as h:
        h.write(
            textwrap.dedent(
                """\
            #pragma once
            #include <cstddef>
            #include <cstdint>
            struct EmbeddedFile {
              const char* name;
              const unsigned char* data;
              std::size_t size;
            };
            """
            )
        )
        for logical, sym, _ in entries:
            h.write(f"extern const unsigned char {sym}[];\n")
            h.write(f"extern const std::size_t {sym}_len;\n")
        h.write(f"\nextern const EmbeddedFile {table_sym}[];\n")
        h.write(f"extern const std::size_t {count_sym};\n")

    # Implementation
    with open(a.out_cc, "w") as cc:
        cc.write(f'#include "{os.path.basename(a.out_h)}"\n\n')
        for logical, sym, data in entries:
            cc.write(f"const unsigned char {sym}[] = {{ {bytes_literal(data)} }};\n")
            cc.write(f"const std::size_t {sym}_len = sizeof({sym});\n\n")
        cc.write(f"const EmbeddedFile {table_sym}[] = {{\n")
        for logical, sym, _ in entries:
            cc.write(f'  {{"{logical}", {sym}, {sym}_len}},\n')
        cc.write("};\n")
        cc.write(f"const std::size_t {count_sym} = {len(entries)};\n")


if __name__ == "__main__":
    main()
