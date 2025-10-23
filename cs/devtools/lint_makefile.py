#!/usr/bin/env python3
"""
makefile_lint.py — Reorganize a Makefile with custom rules:
  - Hoist ALL-CAPS variable assignments to a single, ordering-preserved block near the top
    (while preserving any leading guarded blocks like ifeq/ifdef ... endif).
  - Sort all build rules alphabetically.
  - Ensure every rule header has a "## ..." help comment; if missing, add one.
  - Insert a `.PHONY=thisrule` line immediately before each rule header.
  - Optional safe line-wrapping for comments and simple var assignments.

USAGE:
  python makefile_lint.py [-i] [--wrap WIDTH] [--backup .bak] [PATH]

Defaults:
  - Reads PATH (default: ./Makefile), writes to stdout unless -i is given.
  - Wrapping is DISABLED unless --wrap is specified (integer > 0).

Caveats:
  - Designed to be conservative with Make semantics.
  - Guarded blocks (ifeq/ifneq/ifdef/ifndef ... endif) are preserved in place.
  - Only *top-level* ALL-CAPS var assignments are hoisted.
  - `.PHONY=thisrule` lines are added literally (per the requirement), not `.PHONY:`.
"""

from __future__ import annotations
import argparse
import os
import re
import sys
from typing import List, Tuple, Optional

# --- Regexes ---
RE_GUARD_OPEN = re.compile(r"^\s*(ifeq|ifneq|ifdef|ifndef)\b")
RE_GUARD_CLOSE = re.compile(r"^\s*endif\b")
RE_COMMENT = re.compile(r"^\s*#")
RE_BLANK = re.compile(r"^\s*$")

# A simple variable assignment: FOO ?= bar, FOO = bar, FOO := bar, FOO += bar
RE_VAR_ASSIGN = re.compile(r"^([A-Za-z0-9_][A-Za-z0-9_\.]*)\s*([:+?]?=)\s*(.*)$")

# Target rule header (avoid false-positive on var assignments):  name: deps...
# Disallow '=' before ':'
RE_RULE_HEADER = re.compile(r"^([A-Za-z0-9_./%+\-*?@][^:=]*?)\s*:(?![=]).*$")

# Leading dot directives (not rules): e.g., .PHONY:, .DEFAULT_GOAL :=
RE_DOT_DIRECTIVE = re.compile(r"^\s*\.[A-Za-z0-9_][A-Za-z0-9_.-]*\s*[:]?")

# Recipe line: starts with a tab (or 8 spaces—not standard, but seen). Keep them intact.
RE_RECIPE = re.compile(r"^\t|^( {8,})")

RE_PHONY_LINE = re.compile(r"^\s*\.PHONY\s*([:=])\s*(.*)\s*$")


def _phony_targets_from_line(line: str) -> List[str]:
    m = RE_PHONY_LINE.match(line)
    if not m:
        return []
    names = m.group(2).strip()
    # Split on whitespace; make targets case-sensitive (GNU make is)
    return [t for t in re.split(r"\s+", names) if t]


RE_INLINE_COMMENT_POS = re.compile(r"(?<!\\)#")  # first unescaped '#'
RE_TRAILING_WS = re.compile(r"\s+$")


def _extract_leading_comment_text(prev_lines: list[str]) -> Optional[str]:
    """
    Given a list of immediately preceding lines (in file order), return the first
    comment text to mirror inline on the var line. We scan backward until the first
    non-comment/non-blank line.
    """
    for ln in reversed(prev_lines):
        if RE_BLANK.match(ln):
            continue
        if RE_COMMENT.match(ln):
            # strip leading whitespace + '#' + one optional space
            txt = ln.strip()
            txt = txt[1:].lstrip() if txt.startswith("#") else txt
            return txt.strip()
        break
    return None


def _append_inline_comment_if_missing(
    var_line: str, comment_text: Optional[str]
) -> str:
    """
    Append '  # <comment_text>' to var_line if it has no inline comment already.
    Preserve newline and trailing whitespace behavior.
    """
    if not comment_text:
        return var_line
    nl = "\n" if var_line.endswith("\n") else ""
    core = var_line[:-1] if nl else var_line

    # If var line already has a (non-escaped) '#', do nothing.
    if RE_INLINE_COMMENT_POS.search(core):
        return var_line

    # Keep existing trailing spaces (before adding our "  # ...")
    m = RE_TRAILING_WS.search(core)
    if m:
        stem = core[: m.start()]
        trail = core[m.start() :]
    else:
        stem, trail = core, ""

    return f"{stem}  # {comment_text}{trail}{nl}"


# ALL-CAPS name test
def is_all_caps(name: str) -> bool:
    # Consider ALL CAPS + digits + underscore only to be "ALL CAPS"
    return re.fullmatch(r"[A-Z0-9_]+", name or "") is not None


class Block:
    """A contiguous block of lines with a type label."""

    def __init__(self, kind: str, lines: List[str], key: Optional[str] = None):
        self.kind = kind  # 'guard', 'rule', 'var', 'misc'
        self.lines = lines[:]  # raw lines (with newlines)
        self.key = key  # sort key: for 'rule' (target), for 'var' (var name)


def emit_var_block(b: Block, wrap_width, out_lines):
    # Mirror leading comment (if any) into an inline comment unless var already has one
    ln0 = b.lines[0]
    leading = getattr(b, "leading_context", None)
    mirrored = _append_inline_comment_if_missing(
        ln0, _extract_leading_comment_text(leading or [])
    )
    out = [mirrored] + b.lines[1:]
    if wrap_width > 0:
        for ln in out:
            out_lines.extend(safe_wrap_line(ln, wrap_width))
    else:
        out_lines.extend(out)


def parse_blocks(lines: List[str]) -> List[Block]:
    """Parse Makefile into blocks: leading guard blocks, var assigns, rules, misc."""
    i = 0
    n = len(lines)
    blocks: List[Block] = []
    guard_depth = 0

    def consume_guard(start_idx: int) -> Tuple[int, Block]:
        depth = 0
        out = []
        j = start_idx
        while j < n:
            line = lines[j]
            out.append(line)
            if RE_GUARD_OPEN.match(line):
                depth += 1
            elif RE_GUARD_CLOSE.match(line):
                depth -= 1
                if depth <= 0:
                    j += 1
                    break
            j += 1
        return j, Block("guard", out)

    while i < n:
        line = lines[i]

        # Guarded block
        if RE_GUARD_OPEN.match(line):
            i, blk = consume_guard(i)
            blocks.append(blk)
            continue

        # Blank or comment lines: keep as 'misc' singletons (we will preserve relative order in misc_list)
        if RE_BLANK.match(line) or RE_COMMENT.match(line):
            blocks.append(Block("misc", [line]))
            i += 1
            continue

        # Dot directives that aren't rules (e.g., .DEFAULT_GOAL, .PHONY: x)
        if RE_DOT_DIRECTIVE.match(line) and not RE_RULE_HEADER.match(line):
            blocks.append(Block("misc", [line]))
            i += 1
            continue

        # Variable assignment (single line) — capture leading neighbors for comment mirroring
        mva = RE_VAR_ASSIGN.match(line)
        if mva and not RE_RULE_HEADER.match(line):
            varname = mva.group(1)

            # Collect immediately preceding blank/comment lines (but don't consume them;
            # we only *remember* them so we can duplicate the comment onto the var line).
            prev_idx = len(blocks) - 1
            leading_neighbors: list[str] = []
            while prev_idx >= 0 and blocks[prev_idx].kind == "misc":
                # Only single-line misc blocks are candidates
                if len(blocks[prev_idx].lines) == 1 and (
                    RE_BLANK.match(blocks[prev_idx].lines[0])
                    or RE_COMMENT.match(blocks[prev_idx].lines[0])
                ):
                    leading_neighbors.append(blocks[prev_idx].lines[0])
                    prev_idx -= 1
                    # Keep scanning upward; we stop at first non blank/comment
                    continue
                break
            # Store the leading neighbors (file order) reversed back
            leading_neighbors.reverse()

            blk = Block("var", [line], key=varname)
            # Stash the leading neighbors so we can use them later in render
            blk.leading_context = leading_neighbors  # type: ignore[attr-defined]
            blocks.append(blk)
            i += 1
            continue

        # Rule header + its recipe/body (collect until next top-level header/guard/var)
        mr = RE_RULE_HEADER.match(line)
        if mr:
            target_header = mr.group(1).strip()
            out = [line]
            i += 1
            while i < n:
                nxt = lines[i]
                # Stop if next starts a new header/guard/var/dot-directive and isn't a recipe
                if RE_RULE_HEADER.match(nxt) and not RE_RECIPE.match(nxt):
                    break
                if RE_GUARD_OPEN.match(nxt) and not RE_RECIPE.match(nxt):
                    break
                if (
                    RE_VAR_ASSIGN.match(nxt) or RE_DOT_DIRECTIVE.match(nxt)
                ) and not RE_RECIPE.match(nxt):
                    break
                out.append(nxt)
                i += 1
            blocks.append(Block("rule", out, key=target_header))
            continue

        # Fallback: treat as misc (single line)
        blocks.append(Block("misc", [line]))
        i += 1

    return blocks


def gather_caps_vars(blocks: List[Block]) -> Tuple[List[Block], List[Block]]:
    """Split ALL-CAPS var assignment blocks from the rest (only top-level 'var' blocks)."""
    caps: List[Block] = []
    rest: List[Block] = []
    for b in blocks:
        if b.kind == "var" and b.key and is_all_caps(b.key):
            caps.append(b)
        else:
            rest.append(b)
    return caps, rest


def ensure_rule_has_comment(line: str, target: str) -> str:
    """If rule header lacks '##', append an auto comment."""
    if "##" in line:
        return line
    # Preserve original end-of-line (we operate on raw line with newline)
    if line.endswith("\n"):
        body = line[:-1]
        nl = "\n"
    else:
        body = line
        nl = ""
    if ".PHONY" in line:
        comment = ""
    else:
        comment = f" ## {target} rule (auto)"
    return f"{body}{comment}\n"


def insert_phony_equals(rule_block: Block) -> Block:
    """Ensure a .PHONY line is immediately before the rule header; add only if missing."""
    # If first line is already a .PHONY directive, keep as-is.
    if rule_block.lines and RE_PHONY_LINE.match(rule_block.lines[0]):
        return rule_block
    header = rule_block.lines[0]
    nl = "\n" if header.endswith("\n") else ""
    target = rule_block.key or ""
    phony = f".PHONY={target}{nl}"
    new_lines = [phony] + rule_block.lines
    return Block("rule", new_lines, key=rule_block.key)


def sort_rule_blocks(blocks: List[Block]) -> List[Block]:
    rb = [b for b in blocks if b.kind == "rule"]
    rb.sort(key=lambda b: (b.key or "").lower())
    return rb


def safe_wrap_line(line: str, width: int) -> List[str]:
    """
    Wrap only safe lines:
      - Full-line comments (# ...).
      - Simple var assignments: NAME <op> value (split by spaces), convert to backslash-continued lines.
    Never wrap recipe lines (tabs), guard lines, rule headers, or dot directives.
    """
    if width <= 0:
        return [line]

    if (
        RE_RECIPE.match(line)
        or RE_RULE_HEADER.match(line)
        or RE_GUARD_OPEN.match(line)
        or RE_GUARD_CLOSE.match(line)
        or RE_DOT_DIRECTIVE.match(line)
    ):
        return [line]

    # Comment lines: simple word-wrap at spaces
    if RE_COMMENT.match(line):
        text = line.rstrip("\n")
        out: List[str] = []
        prefix = ""
        # Keep indentation if any, but most comments start at column 0
        m = re.match(r"^(\s*#\s*)", text)
        if m:
            prefix = m.group(1)
            content = text[len(prefix) :]
        else:
            prefix = ""
            content = text

        words = content.split(" ")
        cur = prefix
        for w in words:
            candidate = w if not cur or cur == prefix else " " + w
            if len(cur + candidate) > width and cur != prefix:
                out.append(cur)
                cur = prefix + w
            else:
                cur += candidate
        if cur:
            out.append(cur)
        return [l + "\n" for l in out]

    # Simple var assignments (avoid changing semantics)
    m = RE_VAR_ASSIGN.match(line)
    if m:
        name, op, value = m.group(1), m.group(2), m.group(3).rstrip("\n")
        # Only wrap when value has spaces and is long
        if len(line) <= width or " " not in value:
            return [line]
        # Build wrapped with trailing backslashes; keep one leading space after op for readability
        parts = value.split()
        prefix = f"{name} {op} "
        lines: List[str] = []
        cur = prefix
        for p in parts:
            candidate = ("" if cur.endswith(" ") else " ") + p
            if len(cur + candidate) > width and cur != prefix:
                lines.append(cur + " \\\n")
                cur = (
                    " " * len(prefix) + p
                )  # indent continuation under the first char after op
            else:
                cur += candidate
        lines.append(cur + "\n")
        return lines

    return [line]


def render_makefile(
    blocks: List[Block],
    wrap_width: int = 0,
) -> List[str]:
    """
    Compose the new Makefile:
      - Preserve initial leading guarded blocks exactly in their original positions until we hit a non-guard.
      - Then emit ALL-CAPS var block (ordering-preserved).
      - Then emit remaining misc/dot directives (original order).
      - Then emit sorted rules (with comment + phony insertion).
      - Finally, emit any trailing misc lines that were not already placed (we keep order).
    """
    # Identify initial contiguous guard blocks at the very beginning
    leading: List[Block] = []
    tail: List[Block] = []
    encountered_non_guard = False
    for b in blocks:
        if not encountered_non_guard and b.kind == "guard":
            leading.append(b)
        else:
            encountered_non_guard = True
            tail.append(b)

    caps, tail_rest = gather_caps_vars(tail)

    # Extract rule blocks and non-rule/misc blocks from tail_rest
    rule_blocks = [b for b in tail_rest if b.kind == "rule"]
    other_blocks = [b for b in tail_rest if b.kind != "rule"]

    # Prepare ALL-CAPS vars, sorted by var name (key)
    caps_sorted = caps

    # Prepare sorted rules; ensure comments + .PHONY=thisrule
    sorted_rules = []
    for rb in sort_rule_blocks(rule_blocks):
        # Ensure the header has a ## comment
        header = rb.lines[0]
        tgt = rb.key or ""
        rb.lines[0] = ensure_rule_has_comment(header, tgt)
        # Insert `.PHONY=thisrule` before header
        rb2 = insert_phony_equals(rb)
        sorted_rules.append(rb2)

    # Build final lines with optional wrapping
    out_lines: List[str] = []

    def emit_block(b: Block, wrap_width, out_lines):
        if wrap_width > 0:
            for ln in b.lines:
                out_lines.extend(safe_wrap_line(ln, wrap_width))
        else:
            out_lines.extend(b.lines)

    # 1) Leading guard blocks as-is
    for b in leading:
        emit_block(b, wrap_width, out_lines)

    # 2) ALL-CAPS var block (if any)
    if caps_sorted:
        # Add a separation comment for clarity (non-functional)
        # Keep extremely conservative: only add a blank line if not already blank
        if out_lines and not RE_BLANK.match(out_lines[-1]):
            out_lines.append("\n")
        # Emit
        for b in caps_sorted:
            emit_var_block(
                b, wrap_width, out_lines
            )  # << use this instead of emit_block
        if not out_lines[-1].endswith("\n"):
            out_lines[-1] += "\n"
        # if not RE_BLANK.match(out_lines[-1]):
        #     out_lines.append('\n')

    # 3) Other non-rule blocks, in original relative order
    for b in other_blocks:
        if b.kind == "var":
            emit_var_block(b, wrap_width, out_lines)
        else:
            emit_block(b, wrap_width, out_lines)

    # Ensure a blank line before rules (cosmetic, not semantic)
    if out_lines and not RE_BLANK.match(out_lines[-1]):
        out_lines.append("\n")

    # 4) Sorted rules
    for idx, rb in enumerate(sorted_rules):
        emit_block(rb, wrap_width, out_lines)
        # Keep a blank line between rules for readability
        if (idx != len(sorted_rules) - 1) and (
            not out_lines or not RE_BLANK.match(out_lines[-1])
        ):
            out_lines.append("\n")

    while out_lines[-1] == "\n" and out_lines[-2] == "\n":
        out_lines.pop()

    return out_lines


def merge_leading_phony(blocks: List[Block]) -> List[Block]:
    """
    If a single-line .PHONY directive immediately precedes a rule whose target
    is listed in that directive, attach that .PHONY line to the rule block so
    they remain adjacent through sorting.
    """
    out: List[Block] = []
    i = 0
    while i < len(blocks):
        b = blocks[i]
        if (
            b.kind == "misc"
            and len(b.lines) == 1
            and RE_PHONY_LINE.match(b.lines[0])
            and i + 1 < len(blocks)
            and blocks[i + 1].kind == "rule"
        ):
            rule = blocks[i + 1]
            target = rule.key or ""
            phony_targets = _phony_targets_from_line(b.lines[0])
            if target in phony_targets or "thisrule" in phony_targets:
                # Attach .PHONY line to the rule (placing it immediately before header)
                new_lines = b.lines + rule.lines
                out.append(Block("rule", new_lines, key=rule.key))
                i += 2
                continue
        out.append(b)
        i += 1
    return out


def main():
    ap = argparse.ArgumentParser(
        description="Lint/organize a Makefile per custom rules."
    )
    ap.add_argument(
        "path",
        nargs="?",
        default="Makefile",
        help="Path to Makefile (default: ./Makefile)",
    )
    ap.add_argument(
        "-i",
        "--in-place",
        action="store_true",
        help="Write changes back to file in place.",
    )
    ap.add_argument(
        "--backup", default="", help="Backup extension to use with -i (e.g., .bak)."
    )
    ap.add_argument(
        "--wrap",
        type=int,
        default=0,
        help="Wrap width for safe wrapping (comments/var lines only). Disabled when 0.",
    )
    ap.add_argument("-v", "--verbose", action="store_true", help="Verbose logging.")
    args = ap.parse_args()

    try:
        with open(args.path, "r", encoding="utf-8") as f:
            src_lines = f.readlines()
    except FileNotFoundError:
        print(f"ERROR: file not found: {args.path}", file=sys.stderr)
        sys.exit(2)

    blocks = parse_blocks(src_lines)
    blocks = merge_leading_phony(
        blocks
    )  # <-- keep .PHONY lines attached to following rule

    if args.verbose:
        kinds = [b.kind for b in blocks]
        print(f"[makefile_lint] parsed {len(blocks)} blocks: {kinds}", file=sys.stderr)

    out_lines = render_makefile(blocks, wrap_width=args.wrap)

    output = "".join(out_lines)

    if args.in_place:
        if args.backup:
            bak_path = args.path + args.backup
            try:
                if os.path.exists(bak_path):
                    os.remove(bak_path)
                os.rename(args.path, bak_path)
                if args.verbose:
                    print(f"[makefile_lint] wrote backup: {bak_path}", file=sys.stderr)
            except OSError as e:
                print(
                    f"ERROR: could not create backup {bak_path}: {e}", file=sys.stderr
                )
                sys.exit(3)
        try:
            with open(args.path, "w", encoding="utf-8") as f:
                f.write(output)
        except OSError as e:
            print(f"ERROR: could not write file {args.path}: {e}", file=sys.stderr)
            sys.exit(4)
        if args.verbose:
            print(f"[makefile_lint] updated {args.path}", file=sys.stderr)
    else:
        sys.stdout.write(output)


if __name__ == "__main__":
    main()
