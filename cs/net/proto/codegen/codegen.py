#!/usr/bin/env python3
# cs/net/proto/codegen/codegen.py
import argparse
import os
import re
import shutil
import subprocess
from glob import glob
from os.path import join, dirname
from pathlib import Path

from cs.net.proto.codegen.parser import ParseInputHeader
from cs.net.proto.codegen.codegen_types import ProtoDB
from cs.net.proto.codegen.validator import ValidateProtos
from cs.net.proto.codegen.writers import (
    WriteGeneratedHhFile,
    WriteGeneratedCcFile,
    WriteGeneratedTestFile,
    WriteGeneratedValidationFile,
)
from cs.devtools.enforce_header_guards import detect_guard, path_to_guard, rewrite_guard
from cs.devtools.filenames_in_comments import comment_prefix, has_shebang

PROTOS: ProtoDB = {}


def _repo_root() -> str:
    root = os.path.abspath(__file__)
    for _ in range(5):
        root = os.path.dirname(root)
    return root


def _clang_format_config() -> str:
    return join(_repo_root(), ".clang-format")


def _derive_out_root(gen_dir: str, in_rel: str) -> Path:
    gen_path = Path(gen_dir)
    rel_parent = Path(in_rel).parent
    out_root = gen_path
    for _ in range(len(rel_parent.parts)):
        out_root = out_root.parent
    return out_root


def _line_has_path(line: str, rel: Path) -> bool:
    rel_str = rel.as_posix()
    pattern = r"^\s*(#|//)\s*" + re.escape(rel_str) + r"\s*$"
    return re.match(pattern, line.strip()) is not None


def _path_comment_line(rel: Path) -> str:
    return f"{comment_prefix(rel)} {rel.as_posix()}\n"


def _apply_header_guards(path_pairs: list[tuple[Path, Path]]) -> None:
    for path, logical_rel in path_pairs:
        if not path.exists():
            continue
        data = path.read_text(encoding="utf-8", errors="replace")
        found, _, _, _ = detect_guard(data)
        if not found:
            continue
        expected = path_to_guard(logical_rel)
        if found == expected:
            continue
        new_data = rewrite_guard(data, found, expected)
        if new_data != data:
            path.write_text(new_data, encoding="utf-8")


def _apply_filename_comments(path_pairs: list[tuple[Path, Path]]) -> None:
    for path, logical_rel in path_pairs:
        if not path.exists():
            continue
        data = path.read_text(encoding="utf-8", errors="replace")
        lines = data.splitlines(True)
        desired_line = _path_comment_line(logical_rel)
        if not lines:
            new_lines = [desired_line]
        else:
            insert_at = 1 if has_shebang(lines[0]) else 0
            if len(lines) > insert_at and _line_has_path(lines[insert_at], logical_rel):
                new_lines = lines
            else:
                new_lines = lines[:insert_at] + [desired_line] + lines[insert_at:]
        if "".join(new_lines) != data:
            path.write_text("".join(new_lines), encoding="utf-8")


def _run_clang_format(path_pairs: list[tuple[Path, Path]]) -> None:
    clang_format = shutil.which("clang-format")
    if clang_format is None:
        raise RuntimeError("clang-format not found on PATH.")
    config_path = _clang_format_config()
    if not os.path.exists(config_path):
        raise RuntimeError(f"clang-format config not found at {config_path}")
    repo_root = Path(_repo_root())
    for path, logical_rel in path_pairs:
        if not path.exists():
            continue
        assume_filename = repo_root / logical_rel
        with open(path, "r", encoding="utf-8") as f:
            contents = f.read()
        result = subprocess.run(
            [
                clang_format,
                "-style=file",
                f"-assume-filename={assume_filename}",
            ],
            input=contents,
            text=True,
            capture_output=True,
            check=True,
        )
        with open(path, "w", encoding="utf-8") as f:
            f.write(result.stdout)


def _apply_codegen_linters(gen_dir: str, in_rel: str, out_root_rel: str | None) -> None:
    proto_basename = os.path.basename(in_rel)
    output_names = [
        proto_basename.replace(".proto.hh", ".proto.cc"),
        proto_basename,
        proto_basename.replace(".proto.hh", ".proto_test.cc"),
        proto_basename.replace(".proto.hh", ".validate.hh"),
    ]
    output_paths = [Path(gen_dir) / name for name in output_names]
    if out_root_rel:
        base_rel = Path(out_root_rel) / Path(in_rel).parent / "gencode"
        logical_rels = [base_rel / name for name in output_names]
    else:
        out_root = _derive_out_root(gen_dir, in_rel)
        logical_rels = [path.relative_to(out_root) for path in output_paths]
    path_pairs = list(zip(output_paths, logical_rels))
    header_pairs = []
    for path, logical_rel in path_pairs:
        if path.suffix == ".hh":
            header_pairs.append((path, logical_rel))
    _apply_header_guards(header_pairs)
    _apply_filename_comments(path_pairs)
    _run_clang_format(path_pairs)


def generate_single(
    in_abs: str,
    in_rel: str,
    out_cc: str,
    out_hh: str,
    out_test_cc: str,
    out_root_rel: str | None,
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

    WriteGeneratedHhFile(PROTOS, gen_dir, in_rel, out_root_rel)
    WriteGeneratedCcFile(PROTOS, gen_dir, in_rel, out_root_rel)
    WriteGeneratedTestFile(PROTOS, gen_dir, in_rel, out_root_rel)
    WriteGeneratedValidationFile(PROTOS, gen_dir, in_rel, out_root_rel)
    _apply_codegen_linters(gen_dir, in_rel, out_root_rel)


def generate_scan(
    src_base_dir: str, gen_base_dir: str, out_root_rel: str | None
) -> None:
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
            WriteGeneratedHhFile(PROTOS, gen_dir, in_rel, out_root_rel)
            WriteGeneratedCcFile(PROTOS, gen_dir, in_rel, out_root_rel)
            WriteGeneratedTestFile(PROTOS, gen_dir, in_rel, out_root_rel)
            WriteGeneratedValidationFile(PROTOS, gen_dir, in_rel, out_root_rel)
            _apply_codegen_linters(gen_dir, in_rel, out_root_rel)


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
    ap.add_argument(
        "--out_root_rel",
        help="Repo-root-relative output root for comments/guards (e.g., cs/...).",
    )

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
            args.in_abs,
            args.in_rel,
            args.out_cc,
            args.out_hh,
            args.out_test_cc,
            args.out_root_rel,
        )
        return

    if args.scan:
        if not args.src_base_dir or not args.gen_base_dir:
            ap.error("--scan requires --src_base_dir and --gen_base_dir")
        generate_scan(args.src_base_dir, args.gen_base_dir, args.out_root_rel)
        return


if __name__ == "__main__":
    main()
