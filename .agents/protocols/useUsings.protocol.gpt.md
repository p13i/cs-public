# useUsings Protocol

When invoked (e.g. `useUsings()` or
`useUsings(path/to/file.cc)`), consolidate non-`std::`
fully-qualified type names in a C++ `.cc` file into a single
`namespace {  // use_usings` block with `using ::...;`
declarations, and shorten usages in the body accordingly.

**Critical:** There must be exactly **one** use_usings block
per file. Include symbols under **`::cs::`** (e.g.
`using ::cs::Result;`). In test files (`*_test.cc`,
`*_test.gpt.cc`), also include `::testing::` symbols (e.g.
`using ::testing::Eq;`, `using ::testing::HasSubstr;`). Do
not add usings for other top-level namespaces (e.g.
`using ::db::Get;`). All usings must use the **full
namespace**. Never add a second block with short-name usings
(e.g. `using ::Result;`). Remove any redundant second block.

**Scope restriction:** Only `.cc` files. Do not apply to
`.hh`, `.hpp`, `.cpp`, `.c`, or any other file type or
extension.

## Invocation Behavior

- **With file arguments** (e.g.
  `useUsings("a.cc", "b/b.cc")`): Apply the protocol only to
  the specified files. Skip any path that does not end with
  `.cc` or `.gpt.cc`; report that it was skipped.
- **Without arguments** (e.g. `useUsings()`): Analyze and
  update all `.cc` and `.gpt.cc` source files in the repo
  (respecting `.gitignore`; exclude external, third_party,
  bazel-out, bazel-bin, etc.).

## Scope

- **Target:** C++ `.cc` files only (includes `.gpt.cc`).
- **Exclude:** All other extensions (`.hh`, `.hpp`, `.cpp`,
  `.c`, etc.).
- **Block marker:** `namespace {  // use_usings` …
  `}  // namespace`.
- **Location:** After the last `#include` line, before the
  rest of the file body.

## Symbols to Consolidate

Add `using ::FullyQualifiedName;` for fully-qualified names
that:

1. **`::cs` namespace** – Include symbols under `::cs::`
   (e.g. `::cs::Result`, `::cs::net::http::Request`).
   **`::testing` in test files** – In test files
   (`*_test.cc`, `*_test.gpt.cc`), also include
   `::testing::` symbols (e.g. `::testing::Eq`,
   `::testing::HasSubstr`, `::testing::Invoke`,
   `::testing::Return`). Do **not** leave `::testing::`
   usings bare at file scope; place them in the use_usings
   block. Do **not** add symbols from other top-level
   namespaces (e.g. `::db::Get`, `::gencode::...`,
   `::log::...`).
2. **Exclude `std::`** – Do not add `std::` types (e.g.
   `std::string`, `std::vector`) unless explicitly
   configured otherwise.
3. **Type-like only** – Include symbols whose last segment
   starts with an uppercase letter (e.g. `::cs::Result`,
   `::cs::net::http::Request`).
4. **Exclude:**
   - Member-function-like: last segment lowercase,
     second-last uppercase (e.g. `SomeClass::method`).
   - Operators and destructors: last segment starts with
     `operator` or `~`.
   - Namespace-only: last segment lowercase (e.g.
     `cs::util::di`).
5. **Visible** – Only add symbols whose top-level namespace
   appears in `#include` paths or `namespace` /
   `using namespace` declarations in the file.
   (Pragmatically, if the symbol appears in compilable
   source, it is already visible; an explicit check is
   optional.)
6. **No shadowing conflicts** – Do not add a using if the
   short name would shadow a local definition in the same
   file. Specifically:
   - A file-local function with the same name (e.g., the
     file defines `void Error(const std::string&)`—do not
     add `using ::cs::Error;`).
   - An out-of-line member/function definition that uses the
     full qualifier (e.g., `Result cs::fs::Delete(...)`)—
     shortening would change the definition target.
   - A symbol defined inside a named namespace block later
     in the same translation unit—the `using` declaration at
     file scope cannot see forward definitions.

## Replacement Rules

- Replace fully-qualified type usages with the short (last)
  segment when:
  - The symbol is type-like.
  - No ambiguity: exactly one fully-qualified symbol maps to
    that short name in the block.
- **Type aliases**
  (`using AppContext = cs::util::di::Context<...>`) should
  also be shortened (e.g.
  `using AppContext = Context<...>`). These are NOT import
  declarations and should be treated as normal code for
  shortening purposes.
- Do **not** replace:
  - Inside preprocessor lines.
  - Inside string literals or character literals.
    (Best-effort: the automated script does not currently
    mask string contents; manual review may be needed for
    files that embed qualified names in strings.)
  - Inside line or block comments. (Best-effort: the script
    skips full-line comments and block-comment regions but
    does not split inline comments on mixed code+comment
    lines.)
  - When followed by `::` (e.g. `SomeType::Nested` stays
    as-is if only `SomeType` is shortened).
  - Out-of-line function definitions (e.g.
    `Result cs::fs::Delete(...)` must keep the qualifier).

## Block Format

```
namespace {  // use_usings
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::net::http::Request;
using ::testing::Eq;
using ::testing::HasSubstr;
...
}  // namespace
```

- **Exactly one** block per file; never add a second block.
- **Closing brace:** Use `}  // namespace` (the default);
  never `}  // use_usings`.
- All usings must use the **full namespace** (e.g.
  `using ::cs::Result;`, never `using ::Result;`).
- One `using` per line.
- Sort `::cs::` symbols lexicographically first, then
  `::testing::` symbols (sorted separately, at the end of
  the block).
- Preserve existing `using` lines that pass the filters;
  merge with newly discovered symbols.
- **`::testing::` usings in test files**: Place them in the
  use_usings block alongside `::cs::` symbols. Do not leave
  them bare at file scope. Common symbols: `Eq`, `Gt`,
  `HasSubstr`, `Invoke`, `IsFalse`, `IsTrue`, `Not`,
  `Return`, `StrEq`.
- **Other non-`::cs::` usings already in the block**:
  Preserve them to avoid breaking builds. Do not add new
  usings from other namespaces (e.g. `::db::`, `::log::`).

## Eliminating Short Aliases

Do not introduce short type aliases like `using p3 = ...` or
`using v3 = ...` when the full type name is available
through the use_usings block. Replace existing short aliases
with the canonical type name (e.g. `p3` → `Point3`, `v3` →
`Vector3`, `r3` → `Ray3`, `m4x4` → `Matrix4x4`).

## No `using namespace`

Replace `using namespace cs::foo::bar;` with explicit
`using ::cs::foo::bar::SymbolName;` declarations for each
symbol actually used. This makes dependencies explicit and
avoids unintended name imports.

**Exception:** `using namespace cs::net::html::dom;` may be
kept as a **function-local** directive (inside a function
body, not at file scope). The `dom` namespace exports
single-letter builder functions (`a`, `div`, `p`, `span`,
`body`, `head`, `html`, `meta`, `code`, `title`, `nbsp`)
that would be impractical to import individually.

## No `using ::std::*`

Do not add `using ::std::string;`, `using ::std::vector;`,
etc. Use the `std::` prefix directly in code. Remove any
existing `using ::std::*` declarations.

## No Redundant Function-Local Usings

If a symbol is already in the file-scope use_usings block,
do not repeat it as a function-local using inside a function
body. Remove any such duplicates.

## Conflicting Short Names

When two symbols share the same short name (e.g.
`::cs::util::Context` and `::cs::util::di::Context`), only
add the one that benefits from shortening. If a symbol is
only ever used with `::` member access (e.g.
`Context::Write(...)`), do not add a using for it—the
shortening rule already skips names followed by `::`.

**Recurring example:** `::cs::util::Context` (static utility
with `::Read`/`::Write` methods) vs
`::cs::util::di::Context` (DI context template). Keep only
`::cs::util::di::Context` in the block. Leave
`cs::util::Context::Read(...)` / `::Write(...)` fully
qualified. The automated script will re-add the conflicting
using on each run; it must be manually removed afterward.
Files affected: `data-viewer/main.cc`,
`code-viewer/main.cc`, `blob-viewer/main.gpt.cc`,
`trycopilot.ai/main.cc`, `trycopilot.ai/ui/ui.cc`.

## Block Exclusivity

The `namespace {  // use_usings` block must contain **only**
`using ::...;` declarations. No functions, variables, types,
type aliases, namespace aliases, or other symbols.

If the file has other anonymous-namespace content (helper
functions, static variables, constants, etc.), place them in
a **separate** `namespace { }` block—e.g.
`namespace {  // helpers` or `namespace {  // impl` —not
inside the use_usings block. Do not mix usings with other
declarations in the same block.

## Helper Namespace Placement

Place helper functions, constants, and type aliases in a
file-scope `namespace {  // helpers` block, **outside** any
named namespace. Named namespace blocks should contain only
the public API implementations. Helpers are visible to named
namespaces through normal C++ lookup.

## Test Namespace Placement

Where possible, move tests out of named namespaces (e.g.
`namespace cs::apps::foo {`) into a file-scope anonymous
namespace. Add explicit usings for the types the tests need
(e.g. `using ::cs::apps::foo::protos::Task;`). This keeps
tests decoupled from namespace structure.

If tests heavily use `protos::` sub-namespace types with the
short `protos::` prefix, they may remain inside the named
namespace to avoid excessive usings. Use judgment.

## Agent Instructions

1. **File selection:** If file path(s) are provided as
   arguments, process only those files (skip any that do not
   end with `.cc` or `.gpt.cc`). If no arguments are
   provided, discover all `.cc` and `.gpt.cc` files in the
   repo (excluding external, third_party, bazel-out,
   bazel-bin).
2. **Validation:** For each target file, validate it ends
   with `.cc` or `.gpt.cc`. Skip and report any that do not.
3. Parse the file to find the existing
   `namespace {  // use_usings` block (if any) and the body
   after it. If the block contains non-using content, move
   that content to a file-scope `namespace {  // helpers`
   block.
4. **Remove redundant blocks:** If the body contains another
   namespace block with only `using ::...;` lines
   (especially short-name usings like `using ::Result;`),
   remove that entire block and merge its symbols (as full
   namespaces) into the main use_usings block.
5. **Remove redundant usings:** Delete duplicate usings in
   inner namespaces that are already in the use_usings
   block. Delete function-local usings that duplicate the
   block. Delete `using ::std::*` declarations (use `std::`
   prefix directly). Replace `using namespace cs::...;` with
   explicit per-symbol usings.
6. **Eliminate short aliases:** Replace `using p3 = ...`,
   `using v3 = ...`, etc. with the canonical type name from
   the use_usings block. Remove the alias declarations.
7. Scan the body for fully-qualified symbols; collect those
   that pass the inclusion rules.
8. Merge with existing usings; filter by visibility. **New
   usings** must be under `::cs::` (or `::testing::` in test
   files). Consolidate any bare `::testing::` usings at file
   scope into the use_usings block. Do not add usings from
   other namespaces (e.g. `::db::`, `::log::`). Ensure all
   usings use full namespace (e.g. `::cs::Result`, not
   `::Result`).
9. **Resolve conflicts:** If two symbols share a short name,
   only keep the one that benefits from shortening. Drop
   symbols only used with `::` member access.
10. Build the new block and replace fully-qualified usages
    in the body with short names where unambiguous. Also
    shorten type aliases (`using X = cs::...`).
11. **Restructure namespaces:** Move helper functions and
    constants from inside named namespaces to file-scope
    `namespace {  // helpers` blocks. Where possible, move
    tests out of named namespaces into file-scope anonymous
    namespaces with explicit usings.
12. **Verify using declarations.** After any bulk
    `replace_all` operation, verify that using declarations
    in the block still have their full `::cs::` or
    `::testing::` prefix. A `replace_all` of
    `::testing::HasSubstr` → `HasSubstr` will also corrupt
    `using ::testing::HasSubstr;` → `using HasSubstr;`.
    Always check.
13. Write the updated file. Preserve all other content and
    formatting.

## Post-Application Checklist

After applying the protocol, verify:

- **No redundant blocks:** Exactly one
  `namespace {  // use_usings` … `}  // namespace` block
  with usings. No second block with short-name usings.
- **No `using ::Result::Main`:** Use `Result::Main(...)` in
  `main()`; never `using ::Result::Main`.
- **`::testing::` in test files:** All `::testing::` usings
  are inside the use_usings block, not bare at file scope.
- **No `using ::std::*`:** Use `std::` prefix directly.
- **No short aliases:** No `using p3 = ...`,
  `using v3 = ...` etc. Use canonical type names from the
  block.
- **No `using namespace`:** Replaced with explicit usings
  (exception: function-local
  `using namespace cs::net::html::dom;` is acceptable).
- **No duplicate usings:** No inner-namespace or
  function-local usings that repeat the block.
- **No conflicting short names:** If two symbols share a
  short name, only the useful one is in the block.
- **Type aliases shortened:** `using X = cs::foo::Bar<...>`
  uses the short form `using X = Bar<...>`.
- **Helpers at file scope:** Helper functions and constants
  are in `namespace {  // helpers`, not inside named
  namespaces.
- **Tests decoupled:** Tests are in file-scope anonymous
  namespaces where practical, with explicit usings for
  needed types.
- **Other non-`::cs` symbols:** Never add `using ::db::*`,
  `using ::gencode::*`, `using ::log::*`. Use fully
  qualified names in code (e.g. `db::Get`).
- **Named namespaces:** Anonymous-namespace symbols are
  visible inside named namespaces through normal unqualified
  lookup. Shortening inside named namespaces is permitted
  when unambiguous. If a name conflict arises, prefer fully
  qualified names or namespace-local usings.

## What the Automated Script Misses

The inlined Python script handles the bulk work but requires
a manual follow-up pass. The agent should check for these
after every script run:

1. **Conflicting `Context` usings.** The script re-adds
   `using ::cs::util::Context;` every run because it sees
   `cs::util::Context::Read(...)` in the body. Manually
   remove it from the 5 affected files (see "Conflicting
   Short Names" above).

2. **Type aliases not shortened.** The script skips all
   `using` lines (to protect import declarations) but this
   also skips type aliases like
   `using AppContext = ::cs::util::di::Context<...>`.
   Manually shorten these to
   `using AppContext = Context<...>`.

3. **Duplicate usings in inner namespaces.** The script
   consolidates `using ::cs::` from file scope and anonymous
   namespaces, but does not remove duplicates inside named
   namespace blocks (e.g.
   `namespace cs::apps::foo { namespace { using ::cs::Ok; } }`
   when `Ok` is already in the use_usings block). Manually
   remove these.

4. **`::testing::` usings left bare.** The script only moves
   `using ::cs::` declarations. Bare `using ::testing::Eq;`
   at file scope or in inner anonymous namespaces must be
   manually moved into the use_usings block.

5. **Unshortened `::testing::X` usages.** After adding
   `using ::testing::HasSubstr;` to the block, grep for
   `::testing::HasSubstr` in the body and shorten. Be
   careful with `replace_all` — it can corrupt the using
   declaration itself (e.g. turning
   `using ::testing::HasSubstr;` into `using HasSubstr;`).
   Always verify using declarations still have `::testing::`
   after a bulk replace.

6. **Short aliases (`p3`, `v3`, `r3`).** The script doesn't
   detect or remove these. Grep for `using [a-z][a-z0-9]+ =`
   and replace with canonical names. Watch for `\b` regex
   traps: replacing `r3` with `Ray3` can corrupt `Vector3` →
   `VectoRay3` if the regex matches a substring. Use
   whole-word replacement carefully or do targeted per-file
   edits.

7. **Macro bodies.** The script skips preprocessor lines but
   doesn't examine macro bodies for shortenable names. Check
   `#define` blocks manually for fully-qualified names that
   could use the short form.

8. **`Object::KVMap`, `Object::NewString`, etc.** The script
   won't shorten `cs::net::json::Object::NewString(...)` to
   `Object::NewString(...)` because the `(?!::)` lookahead
   prevents matching `cs::net::json::Object` when followed
   by `::`. These member-access patterns must be shortened
   manually.

## Invocation Examples

- `useUsings()` – Process all `.cc` and `.gpt.cc` files in
  the repo.
- `useUsings("path/to/file.cc")` – Process only the given
  file.
- `useUsings("a.cc", "b/b.gpt.cc")` – Process only the given
  files.

## Automation Script

The following Python script implements the bulk
`useUsings()` (no-arguments) invocation. It processes all
`.cc` files, adds/populates use_usings blocks, consolidates
scattered `using ::cs::` declarations, shortens usages, and
enforces block exclusivity. It includes safety checks beyond
this protocol (shadowing detection, out-of-line definition
detection, forward-reference detection) to prevent build
breakage.

Known best-effort gaps:

- String/char literals are not masked during shortening.
- Inline comments on mixed code+comment lines may be
  shortened.
- File-argument mode is not yet supported.

To run: save to a `.gpt.py` file and execute with
`python3 <script>`, or have the agent write it to a temp
file and run it.

```python
#!/usr/bin/env python3
"""Comprehensive useUsings protocol: add/consolidate use_usings blocks and shorten usages.

For each .cc file:
1. Find/create the namespace { // use_usings } block
2. Consolidate all ::cs:: using declarations into it (from file scope and anonymous namespaces)
3. Populate with discovered ::cs:: type symbols from the body
4. Shorten fully-qualified usages
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

# Pattern to find cs:: type-like symbols in code
# Matches: cs::ns1::ns2::Type or ::cs::ns1::ns2::Type
# Only lowercase namespace segments followed by one uppercase type segment
CS_TYPE_PAT = re.compile(
    r"(?<![a-zA-Z0-9_:])"
    r"(?:::)?cs::(?:[a-z_][a-zA-Z0-9_]*::)*[A-Z][a-zA-Z0-9_]*"
)


def collect_cs_types(text: str) -> set[str]:
    """Collect ::cs:: type-like symbols from text, skipping comments and preprocessor."""
    symbols: set[str] = set()
    in_block_comment = False

    for line in text.split("\n"):
        stripped = line.lstrip()

        if in_block_comment:
            if "*/" in line:
                in_block_comment = False
            continue

        if stripped.startswith("#"):
            continue

        work_line = line
        if "/*" in work_line:
            work_line = work_line[: work_line.index("/*")]
            in_block_comment = True

        if "//" in work_line:
            work_line = work_line[: work_line.index("//")]

        for m in CS_TYPE_PAT.finditer(work_line):
            sym = m.group(0)
            full = sym if sym.startswith("::") else "::" + sym
            symbols.add(full)

    return symbols


def build_short_map(symbols: list[str]) -> dict[str, str]:
    """Map full_name -> short_name for unambiguous symbols only."""
    short_to_full: dict[str, list[str]] = {}
    for full in symbols:
        short = full.split("::")[-1]
        short_to_full.setdefault(short, []).append(full)
    result = {}
    for short, fulls in short_to_full.items():
        if len(fulls) == 1:
            result[fulls[0]] = short
    return result


def shorten_content(content: str, full_to_short: dict[str, str]) -> str:
    """Replace full names with short names. Skip use_usings block, using lines, preprocessor, comments."""
    if not full_to_short:
        return content

    replacements = sorted(full_to_short.items(), key=lambda x: -len(x[0]))
    lines = content.split("\n")
    result = []
    in_block_comment = False
    in_use_usings = False

    for line in lines:
        stripped = line.lstrip()

        # Track use_usings block - do NOT replace inside it
        if "namespace {" in line and "use_usings" in line:
            in_use_usings = True
        if in_use_usings:
            result.append(line)
            if stripped.startswith("}") and "// namespace" in line:
                in_use_usings = False
            continue

        # Skip ALL using declarations - must keep full qualified names
        if re.match(r"\s*using\s+", line):
            result.append(line)
            continue

        # Skip preprocessor
        if stripped.startswith("#"):
            result.append(line)
            continue

        # Track block comments
        if in_block_comment:
            result.append(line)
            if "*/" in line:
                in_block_comment = False
            continue

        if "/*" in stripped:
            in_block_comment = True
            result.append(line)
            continue

        # Skip line comments
        if stripped.startswith("//"):
            result.append(line)
            continue

        new_line = line
        for full, short in replacements:
            # Replace ::cs::Foo with short name
            escaped = re.escape(full)
            pat = r"(?<![a-zA-Z0-9_:])" + escaped + r"(?!::)"
            new_line = re.sub(pat, short, new_line)
            # Also replace without leading :: (cs::Foo)
            if full.startswith("::"):
                alt = full[2:]
                pat_alt = r"(?<![a-zA-Z0-9_:])" + re.escape(alt) + r"(?!::)"
                new_line = re.sub(pat_alt, short, new_line)
        result.append(new_line)

    return "\n".join(result)


def extract_multiline_using(lines: list[str], start: int) -> tuple[str | None, int]:
    """Extract a using ::cs:: symbol that may span multiple lines.

    Returns (symbol, num_extra_lines). symbol is None if not a cs using.
    num_extra_lines is how many continuation lines beyond the first.
    """
    line = lines[start]
    # Single-line: using ::cs::foo::Bar;
    m = re.match(r"\s*using\s+(::cs::[^;]+);", line)
    if m:
        return m.group(1).strip(), 0

    # Multi-line: using ::cs::foo::bar::
    #                 Baz;
    # or even 3+ lines
    if re.match(r"\s*using\s+::cs::", line) and not line.rstrip().endswith(";"):
        combined = line.rstrip()
        extra = 0
        for j in range(start + 1, min(start + 5, len(lines))):
            combined += lines[j].strip()
            extra += 1
            if lines[j].rstrip().endswith(";"):
                m2 = re.match(r"\s*using\s+(::cs::[^;]+);", combined)
                if m2:
                    # Clean up any internal whitespace in the symbol
                    sym = re.sub(r"\s+", "", m2.group(1).strip())
                    return sym, extra
                break

    return None, 0


def process_file(path: Path) -> bool:
    """Apply useUsings protocol to a single .cc file. Returns True if modified."""
    content = path.read_text()
    lines = content.split("\n")

    # ---- Phase 1: Parse file structure ----

    last_include_idx = -1
    use_usings_start = -1
    use_usings_end = -1
    existing_uu_symbols: list[str] = []

    # Find last #include and existing use_usings block
    for i, line in enumerate(lines):
        if line.lstrip().startswith("#include"):
            last_include_idx = i
        if "namespace {" in line and "use_usings" in line:
            use_usings_start = i
    # Non-using content found inside the use_usings block (needs to be moved out)
    uu_non_using_lines: list[str] = []

    if use_usings_start >= 0:
        for j in range(use_usings_start + 1, len(lines)):
            if lines[j].lstrip().startswith("}") and "// namespace" in lines[j]:
                use_usings_end = j
                break
        if use_usings_end >= 0:
            j = use_usings_start + 1
            while j < use_usings_end:
                sym, extra = extract_multiline_using(lines, j)
                if sym is not None:
                    existing_uu_symbols.append(sym)
                    j += extra
                elif re.match(r"\s*using\s+", lines[j]):
                    # Non-cs using (e.g., using ::testing::Eq;) - track as existing
                    m_other = re.match(r"\s*using\s+(::[^;]+);", lines[j])
                    if m_other:
                        existing_uu_symbols.append(m_other.group(1).strip())
                    else:
                        # Type alias or other using - move to helpers block
                        uu_non_using_lines.append(lines[j])
                elif lines[j].strip():
                    # Non-using, non-blank content - needs to be moved out
                    uu_non_using_lines.append(lines[j])
                j += 1

    has_uu_block = use_usings_start >= 0 and use_usings_end >= 0

    if last_include_idx < 0 and not has_uu_block:
        return False  # No includes and no block - skip

    # ---- Phase 2: Find ::cs:: usings outside the use_usings block ----

    # Track namespace context to determine if usings should be moved
    ns_stack: list[str] = []  # "named" or "anonymous"
    lines_to_remove: set[int] = set()
    moved_symbols: list[str] = []

    i = 0
    while i < len(lines):
        # Skip inside use_usings block
        if has_uu_block and use_usings_start <= i <= use_usings_end:
            i += 1
            continue

        line = lines[i]
        stripped = line.lstrip()

        # Detect namespace opening
        if re.match(r"\s*namespace\s+\w", line) and "{" in line:
            ns_stack.append("named")
        elif re.match(r"\s*namespace\s*\{", line) and "use_usings" not in line:
            ns_stack.append("anonymous")

        # Detect namespace closing (heuristic: }  // namespace or } // namespace)
        if stripped.startswith("}") and "// namespace" in line:
            if ns_stack:
                ns_stack.pop()

        # Check for using ::cs:: declarations to move
        in_named_ns = any(ns == "named" for ns in ns_stack)
        if not in_named_ns:
            sym, extra = extract_multiline_using(lines, i)
            if sym is not None:
                moved_symbols.append(sym)
                for k in range(i, i + extra + 1):
                    lines_to_remove.add(k)
                i += extra  # Skip continuation lines

        i += 1

    # ---- Phase 3: Collect cs:: type symbols from the body ----

    # Build body text excluding use_usings block and lines we're removing
    body_parts = []
    for i, line in enumerate(lines):
        if has_uu_block and use_usings_start <= i <= use_usings_end:
            continue
        if i in lines_to_remove:
            continue
        body_parts.append(line)
    body_text = "\n".join(body_parts)

    found_symbols = collect_cs_types(body_text)

    # ---- Phase 4: Merge all symbols ----

    # Keep existing non-cs usings that were already in the block (don't remove them)
    existing_non_cs = [s for s in existing_uu_symbols if not s.startswith("::cs::")]

    # For new/moved symbols, only include ::cs:: ones
    cs_symbols = sorted(
        set(s for s in existing_uu_symbols if s.startswith("::cs::"))
        | set(s for s in moved_symbols if s.startswith("::cs::"))
        | set(s for s in found_symbols if s.startswith("::cs::"))
    )

    # Combine: cs symbols first (sorted), then existing non-cs (sorted, at end)
    all_symbols = cs_symbols + sorted(existing_non_cs)

    if not all_symbols and not has_uu_block and not lines_to_remove:
        return False  # Nothing to do

    # ---- Phase 4b: Exclude NEWLY DISCOVERED symbols that conflict ----
    # Only apply conflict detection to found_symbols (new discoveries),
    # NOT to existing_uu_symbols or moved_symbols (they were already working)
    body_full_text = "\n".join(body_parts)
    new_conflicts: set[str] = set()

    for sym in found_symbols:
        if not sym.startswith("::cs::"):
            continue
        short = sym.split("::")[-1]
        ns_path = sym[2:]  # remove leading ::

        # Check 1: Local function definitions that shadow the short name
        for line in body_parts:
            stripped = line.lstrip()
            if stripped.startswith("#") or stripped.startswith("//"):
                continue
            if re.search(
                r"(?:void|int|bool|auto|inline\s+void|inline\s+auto)\s+"
                + re.escape(short)
                + r"\s*\(",
                stripped,
            ):
                new_conflicts.add(sym)
                break

        # Check 2: Out-of-line function definitions using full qualifier
        # e.g., "Result cs::fs::Delete(std::string path) {"
        escaped_ns = re.escape(ns_path)
        out_of_line_pat = re.compile(
            r"[\w:*&<>]+\s+" + escaped_ns + r"\s*\("
        )
        if out_of_line_pat.search(body_full_text):
            new_conflicts.add(sym)

        # Check 3: Function defined inside a named namespace block in this file
        # using declaration at file scope can't see forward definitions
        parts = ns_path.split("::")
        if len(parts) >= 2:
            ns_prefix = "::".join(parts[:-1])
            func_name = parts[-1]
            ns_pat = re.compile(
                r"namespace\s+" + re.escape(ns_prefix) + r"\s*\{"
            )
            if ns_pat.search(body_full_text):
                # Check for function/type definition with this name
                func_def_pat = re.compile(
                    r"(?:^|\n)\s*[\w:<>&*,\s]+\s+"
                    + re.escape(func_name)
                    + r"\s*\("
                )
                if func_def_pat.search(body_full_text):
                    new_conflicts.add(sym)

    # Remove conflicting NEW symbols only
    if new_conflicts:
        found_symbols -= new_conflicts
        # Rebuild all_symbols
        cs_symbols = sorted(
            set(s for s in existing_uu_symbols if s.startswith("::cs::"))
            | set(s for s in moved_symbols if s.startswith("::cs::"))
            | set(s for s in found_symbols if s.startswith("::cs::"))
        )
        all_symbols = cs_symbols + sorted(existing_non_cs)

    # ---- Phase 5: Remove old using lines and clean up empty blocks ----

    new_lines: list[str] = []
    for i, line in enumerate(lines):
        if i in lines_to_remove:
            continue
        # Skip the old use_usings block (we'll rebuild it)
        if has_uu_block and use_usings_start <= i <= use_usings_end:
            continue
        new_lines.append(line)

    # Clean up empty anonymous namespace blocks left after removing usings
    cleaned_lines: list[str] = []
    i = 0
    while i < len(new_lines):
        # Check for empty namespace { ... } // namespace
        if re.match(r"\s*namespace\s*\{", new_lines[i]) and "use_usings" not in new_lines[i]:
            # Look ahead for closing brace
            block_lines = [new_lines[i]]
            j = i + 1
            only_blank = True
            while j < len(new_lines):
                block_lines.append(new_lines[j])
                s = new_lines[j].strip()
                if s and s != "":
                    if s.startswith("}") and "// namespace" in new_lines[j]:
                        # Check if block is empty (only blank lines between open and close)
                        if only_blank:
                            # Remove the entire empty block
                            # Also remove a preceding blank line if present
                            if cleaned_lines and cleaned_lines[-1].strip() == "":
                                cleaned_lines.pop()
                            # Also skip a following blank line
                            if j + 1 < len(new_lines) and new_lines[j + 1].strip() == "":
                                j += 1
                            i = j + 1
                            break
                        else:
                            # Block has content, keep it
                            for bl in block_lines:
                                cleaned_lines.append(bl)
                            i = j + 1
                            break
                    else:
                        only_blank = False
                j += 1
            else:
                # Didn't find closing brace, keep lines as-is
                for bl in block_lines:
                    cleaned_lines.append(bl)
                i = j
            continue
        cleaned_lines.append(new_lines[i])
        i += 1

    new_lines = cleaned_lines

    # ---- Phase 6: Build and insert use_usings block ----

    if all_symbols:
        block_content = ["namespace {  // use_usings"]
        for sym in all_symbols:
            block_content.append(f"using {sym};")
        block_content.append("}  // namespace")
    else:
        block_content = ["namespace {  // use_usings", "}  // namespace"]

    # Build the extra anonymous namespace block for non-using content moved out
    extra_ns_lines: list[str] = []
    if uu_non_using_lines:
        extra_ns_lines = ["", "namespace {  // helpers"] + uu_non_using_lines + ["}  // namespace"]

    # Determine insertion point
    if has_uu_block:
        # Insert at the position where the old use_usings block was
        # We need to find this position in new_lines (after removals)
        # Count lines before the old block that survived removal
        insert_idx = 0
        orig_idx = 0
        for orig_idx in range(use_usings_start):
            if orig_idx not in lines_to_remove:
                insert_idx += 1
    else:
        # Insert after the last #include in the cleaned lines
        insert_idx = -1
        for i, line in enumerate(new_lines):
            if line.lstrip().startswith("#include"):
                insert_idx = i
        if insert_idx < 0:
            insert_idx = 0
            for i, line in enumerate(new_lines):
                if line.lstrip().startswith("//"):
                    insert_idx = i + 1
                else:
                    break
        insert_idx += 1  # Insert AFTER the last include

    # Insert with blank line before and after
    if insert_idx < len(new_lines) and new_lines[insert_idx].strip() == "":
        final_lines = (
            new_lines[:insert_idx]
            + [""]
            + block_content
            + extra_ns_lines
            + [""]
            + new_lines[insert_idx + 1 :]
        )
    else:
        final_lines = (
            new_lines[:insert_idx]
            + [""]
            + block_content
            + extra_ns_lines
            + [""]
            + new_lines[insert_idx:]
        )

    # ---- Phase 7: Shorten usages ----

    new_content = "\n".join(final_lines)
    # Only shorten ::cs:: symbols (not ::testing:: etc.)
    cs_only = [s for s in all_symbols if s.startswith("::cs::")]
    full_to_short = build_short_map(cs_only)
    new_content = shorten_content(new_content, full_to_short)

    if new_content != content:
        path.write_text(new_content)
        return True
    return False


def main() -> int:
    root = Path("/workspaces/cs")
    skip_patterns = ["bazel-", "third_party", "external", "codegen/testdata/golden"]

    count = 0
    all_files = sorted(set(root.rglob("*.cc")))
    for p in all_files:
        rel = str(p.relative_to(root))
        if any(x in rel for x in skip_patterns):
            continue
        try:
            if process_file(p):
                count += 1
                print(rel)
        except Exception as e:
            print(f"ERROR {rel}: {e}", file=sys.stderr)

    print(f"\nModified {count} files", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
```
