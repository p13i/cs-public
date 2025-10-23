#!/usr/bin/env python3
"""
use_usings.py — Consolidate fully-qualified names into a single anonymous-namespace
                using-block and safely shorten usages in C++ .cc files.
"""
from __future__ import annotations
import re
import sys
from pathlib import Path
from collections import defaultdict, OrderedDict
from typing import List, Tuple, Dict, Set, Optional

# Flag to include STL container types (like std::vector) in aliasing
INCLUDE_STD_VECTOR_TYPES: bool = False

# Patterns
FQ_SYMBOL_RE = re.compile(r"\b(?:[A-Za-z_]\w*(?:::[A-Za-z_]\w+)+)\b")
INCLUDE_LINE_RE = re.compile(r"^\s*#\s*include\b.*$", re.MULTILINE)
USING_LINE_RE = re.compile(r"^\s*using\s+::([A-Za-z_][\w:]*\w)\s*;\s*$", re.MULTILINE)
BLOCK_START_RE = re.compile(r"^\s*namespace\s*\{\s*//\s*use_usings\s*$", re.MULTILINE)
BLOCK_END_RE = re.compile(r"^\s*\}\s*//\s*use_usings\s*$", re.MULTILINE)
NAMESPACE_DECL_RE = re.compile(r"^\s*namespace\s+[A-Za-z_:]+\s*\{", re.MULTILINE)


def is_std_symbol(fq: str) -> bool:
    if not fq.startswith("std::"):
        return False
    if INCLUDE_STD_VECTOR_TYPES and fq.startswith("std::vector"):
        return False
    return True


def last_segment(fq: str) -> str:
    return fq.split("::")[-1]


def second_last_segment(fq: str) -> Optional[str]:
    parts = fq.split("::")
    return parts[-2] if len(parts) >= 2 else None


def is_type_like(fq: str) -> bool:
    seg = last_segment(fq)
    return bool(seg and seg[0].isupper())


def is_member_function_like(fq: str) -> bool:
    last = last_segment(fq)
    prev = second_last_segment(fq)
    return bool(last and prev and last[0].islower() and prev[0].isupper())


def is_operator_or_dtor(fq: str) -> bool:
    last = last_segment(fq)
    return last.startswith("operator") or last.startswith("~")


def is_namespace_only(fq: str) -> bool:
    return fq.endswith("::") or last_segment(fq)[0].islower()


def find_last_include_pos(code: str) -> int:
    matches = list(INCLUDE_LINE_RE.finditer(code))
    return matches[-1].end() if matches else 0


def find_existing_block_span(code: str) -> Optional[Tuple[int, int]]:
    m_start = BLOCK_START_RE.search(code)
    if not m_start:
        return None
    m_end = BLOCK_END_RE.search(code, m_start.end())
    if not m_end:
        return None
    return (m_start.start(), m_end.end())


def parse_existing_usings(block_text: str) -> Set[str]:
    return set(USING_LINE_RE.findall(block_text))


def remove_all_usings(text: str) -> str:
    return USING_LINE_RE.sub("", text)


def split_header_block_body(code: str) -> Tuple[str, str, str, Set[str]]:
    span = find_existing_block_span(code)
    if span:
        start, end = span
        header, block, body = code[:start], code[start:end], code[end:]
        return header, block, body, parse_existing_usings(block)
    pos = find_last_include_pos(code)
    return code[:pos], "", code[pos:], set()


def collect_fq_symbols(text: str) -> List[str]:
    symbols = FQ_SYMBOL_RE.findall(text)
    symbols = [s for s in symbols if not is_std_symbol(s)]
    return list(OrderedDict.fromkeys(symbols))


def compute_final_symbol_sets(
    existing: Set[str], found: List[str]
) -> Tuple[Set[str], Dict[str, str]]:
    filtered_found = [
        fq
        for fq in found
        if not is_member_function_like(fq)
        and not is_operator_or_dtor(fq)
        and not is_namespace_only(fq)
    ]
    existing_filtered = {
        fq
        for fq in existing
        if not is_std_symbol(fq)
        and not is_member_function_like(fq)
        and not is_operator_or_dtor(fq)
        and not is_namespace_only(fq)
    }
    combined = set(existing_filtered) | set(filtered_found)
    by_short: Dict[str, List[str]] = defaultdict(list)
    for fq in combined:
        by_short[last_segment(fq)].append(fq)

    final_usings: Set[str] = set(existing_filtered)
    existing_shorts = {last_segment(fq) for fq in existing_filtered}
    for fq in filtered_found:
        short = last_segment(fq)
        if len(by_short[short]) == 1 and short not in existing_shorts:
            final_usings.add(fq)

    fq_to_short: Dict[str, str] = {}
    by_short_final: Dict[str, List[str]] = defaultdict(list)
    for fq in final_usings:
        by_short_final[last_segment(fq)].append(fq)
    for fq in final_usings:
        short = last_segment(fq)
        if is_type_like(fq) and len(by_short_final[short]) == 1:
            fq_to_short[fq] = short

    if INCLUDE_STD_VECTOR_TYPES:
        for fq in final_usings:
            if fq.startswith("std::vector"):
                fq_to_short[fq] = last_segment(fq)

    return final_usings, fq_to_short


def build_using_block(symbols: Set[str]) -> str:
    if not symbols:
        return ""
    lines = ["", "", "namespace {  // use_usings"]
    for s in sorted(symbols):
        lines.append(f"using ::{s};")
    lines.append("}  // use_usings")
    lines.append("")
    return "\n".join(lines)


def compute_replacement_spans(text: str) -> List[Tuple[int, int]]:
    spans: List[Tuple[int, int]] = []
    i, n = 0, len(text)
    NORMAL, LINE_CMT, BLOCK_CMT, STR, CHR = range(5)
    state = NORMAL
    span_start = 0
    escape = False
    preproc_line_starts: Set[int] = {
        m.start()
        for m in re.finditer(r"(?m)^[^\n]*$", text)
        if m.group(0).lstrip().startswith("#")
    }

    def at_preproc_line(idx: int) -> bool:
        line_start = text.rfind("\n", 0, idx) + 1
        return line_start in preproc_line_starts

    while i < n:
        c = text[i]
        c2 = text[i + 1] if i + 1 < n else ""
        if state == NORMAL:
            if at_preproc_line(i):
                if span_start < i:
                    spans.append((span_start, i))
                end = text.find("\n", i)
                if end == -1:
                    break
                i = end + 1
                span_start = i
                continue
            if c == "/" and c2 == "/":
                if span_start < i:
                    spans.append((span_start, i))
                state = LINE_CMT
                i += 2
                continue
            if c == "/" and c2 == "*":
                if span_start < i:
                    spans.append((span_start, i))
                state = BLOCK_CMT
                i += 2
                continue
            if c == '"':
                if span_start < i:
                    spans.append((span_start, i))
                state = STR
                escape = False
                i += 1
                continue
            if c == "'":
                if span_start < i:
                    spans.append((span_start, i))
                state = CHR
                escape = False
                i += 1
                continue
            i += 1
        elif state == LINE_CMT:
            if c == "\n":
                state = NORMAL
                span_start = i + 1
            i += 1
        elif state == BLOCK_CMT:
            if c == "*" and c2 == "/":
                state = NORMAL
                i += 2
                span_start = i
                continue
            i += 1
        elif state == STR:
            if c == "\\" and not escape:
                escape = True
                i += 1
                continue
            if c == '"' and not escape:
                state = NORMAL
                i += 1
                span_start = i
                continue
            escape = False
            i += 1
        elif state == CHR:
            if c == "\\" and not escape:
                escape = True
                i += 1
                continue
            if c == "'" and not escape:
                state = NORMAL
                i += 1
                span_start = i
                continue
            escape = False
            i += 1
    if state == NORMAL and span_start < n:
        spans.append((span_start, n))
    return spans


def replace_types_with_shorts(body: str, fq_to_short: Dict[str, str]) -> str:
    if not fq_to_short:
        return body
    items = sorted(fq_to_short.items(), key=lambda kv: len(kv[0]), reverse=True)
    pattern = re.compile(
        r"\b(?:" + "|".join(re.escape(fq) for fq, _ in items) + r")\b(?!\s*::)"
    )
    spans = compute_replacement_spans(body)
    if not spans:
        return body
    result_chunks: List[str] = []
    last_end = 0
    for start, end in spans:
        if last_end < start:
            result_chunks.append(body[last_end:start])
        segment = body[start:end]
        segment = pattern.sub(
            lambda m: fq_to_short.get(m.group(0), m.group(0)), segment
        )
        result_chunks.append(segment)
        last_end = end
    if last_end < len(body):
        result_chunks.append(body[last_end:])
    return "".join(result_chunks)


def use_usings(path: Path) -> None:
    text = path.read_text(encoding="utf-8")
    header, block, body, existing_symbols = split_header_block_body(text)
    found_symbols = collect_fq_symbols(body)

    visible_namespaces: Set[str] = set()
    for inc in INCLUDE_LINE_RE.findall(text):
        visible_namespaces.update(re.findall(r"[A-Za-z_][\w_]*", inc))
    for ns_match in re.finditer(r"\bnamespace\s+([A-Za-z_][\w_]*)", text):
        visible_namespaces.add(ns_match.group(1))
    for using_ns in re.finditer(r"\busing\s+namespace\s+([A-Za-z_][\w_:]*)", text):
        visible_namespaces.update(using_ns.group(1).split("::"))

    final_usings, fq_to_short = compute_final_symbol_sets(
        existing_symbols, found_symbols
    )
    final_usings = {
        fq for fq in final_usings if fq.split("::", 1)[0] in visible_namespaces
    }
    fq_to_short = {fq: short for fq, short in fq_to_short.items() if fq in final_usings}

    new_block = build_using_block(final_usings)
    updated_body = replace_types_with_shorts(body, fq_to_short)
    updated_text = header + new_block + updated_body

    if updated_text != text:
        path.write_text(updated_text, encoding="utf-8")
        print(
            f"[updated] {path} (+{len(final_usings)} usings, {len(fq_to_short)} substitutions)"
        )
    else:
        print(f"[no change] {path}")


def main(argv: List[str]) -> int:
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--repo_root", "-R", help="Root directory to search for .cc files"
    )
    parser.add_argument("--include_std_vector", "-V", action="store_true")
    parser.add_argument("files", nargs="*")
    args = parser.parse_args(argv[1:])
    global INCLUDE_STD_VECTOR_TYPES
    INCLUDE_STD_VECTOR_TYPES = args.include_std_vector

    files_to_process: List[Path] = []
    if args.repo_root:
        root = Path(args.repo_root)
        for p in root.rglob("*.cc"):
            if not p.is_file():
                continue
            if any(
                x in p.parts
                for x in ("external", "third_party", "bazel-out", "bazel-bin")
            ):
                continue
            files_to_process.append(p)
    for f in args.files:
        p = Path(f)
        if p.is_file():
            files_to_process.append(p)
    if not files_to_process:
        parser.print_usage()
        return 2
    for path in files_to_process:
        use_usings(path)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
