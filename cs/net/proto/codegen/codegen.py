#!/usr/bin/env python3
import argparse
import os
from os.path import join, dirname
from glob import glob

from cs.net.proto.codegen.parser import ParseInputHeader
from cs.net.proto.codegen.codegen_types import ProtoDB
from cs.net.proto.codegen.validator import ValidateProtos
from cs.net.proto.codegen.writers import (
    WriteGeneratedHhFile,
    WriteGeneratedCcFile,
    WriteGeneratedTestFile,
)

PROTOS: ProtoDB = {}


def generate_single(
    in_abs: str, in_rel: str, out_cc: str, out_hh: str, out_test_cc: str
) -> None:
    with open(in_abs, "r") as f:
        global PROTOS
        PROTOS = ParseInputHeader(f.read(), filename=in_rel)
    assert ValidateProtos(PROTOS)

    for p in (out_cc, out_hh, out_test_cc):
        os.makedirs(os.path.dirname(p), exist_ok=True)

    gen_dir = os.path.dirname(out_cc)
    if not gen_dir.endswith("gencode"):
        gen_dir = join(gen_dir, "gencode")

    WriteGeneratedHhFile(PROTOS, gen_dir, in_rel)
    WriteGeneratedCcFile(PROTOS, gen_dir, in_rel)
    WriteGeneratedTestFile(PROTOS, gen_dir, in_rel)


def generate_scan(src_base_dir: str, gen_base_dir: str) -> None:
    filepaths = glob(join(src_base_dir, "cs/**/protos/*.proto.hh"), recursive=True)
    filepaths = [f[len(src_base_dir) :].lstrip("/") for f in filepaths]
    filepaths = [f for f in filepaths if "gencode" not in f]

    packages_to_filenames = {}
    for rel in filepaths:
        package = dirname(dirname(rel))
        packages_to_filenames.setdefault(package, []).append(rel)

    for _, rel_list in packages_to_filenames.items():
        for in_rel in rel_list:
            with open(join(src_base_dir, in_rel), "r") as f:
                global PROTOS
                PROTOS = ParseInputHeader(f.read(), filename=in_rel)
            assert ValidateProtos(PROTOS)
            gen_dir = join(gen_base_dir, dirname(in_rel), "gencode")
            os.makedirs(gen_dir, exist_ok=True)
            WriteGeneratedHhFile(PROTOS, gen_dir, in_rel)
            WriteGeneratedCcFile(PROTOS, gen_dir, in_rel)
            WriteGeneratedTestFile(PROTOS, gen_dir, in_rel)


def main() -> None:
    ap = argparse.ArgumentParser(description="Custom proto.hh -> C++ generator")
    mode = ap.add_mutually_exclusive_group(required=True)
    mode.add_argument(
        "--single",
        action="store_true",
        help="Generate for a single input with explicit output paths.",
    )
    mode.add_argument(
        "--scan",
        action="store_true",
        help="Scan a source tree for *.proto.hh (legacy/offline mode).",
    )

    ap.add_argument("--in_abs", help="Absolute path to input file (.proto.hh)")
    ap.add_argument("--in_rel", help="Workspace-relative path to input file (naming)")
    ap.add_argument("--out_cc", help="Absolute path to write <stem>.proto.cc")
    ap.add_argument("--out_hh", help="Absolute path to write <stem>.proto.hh")
    ap.add_argument("--out_test_cc", help="Absolute path to write <stem>_test.proto.hh")
    ap.add_argument("--src_base_dir", help="Repository root (for --scan)")
    ap.add_argument("--gen_base_dir", help="Output root (for --scan)")

    args = ap.parse_args()

    if args.single:
        required = [
            args.in_abs,
            args.in_rel,
            args.out_cc,
            args.out_hh,
            args.out_test_cc,
        ]
        if not all(required):
            ap.error(
                "--single requires --in_abs, --in_rel, --out_cc, --out_hh, --out_test_cc"
            )
        generate_single(
            args.in_abs, args.in_rel, args.out_cc, args.out_hh, args.out_test_cc
        )
        return

    if args.scan:
        if not args.src_base_dir or not args.gen_base_dir:
            ap.error("--scan requires --src_base_dir and --gen_base_dir")
        generate_scan(args.src_base_dir, args.gen_base_dir)
        return


if __name__ == "__main__":
    main()
