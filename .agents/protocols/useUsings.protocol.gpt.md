# useUsings Protocol

When invoked (e.g. `useUsings()` or
`useUsings(path/to/file.cc)`), consolidate non-`std::`
fully-qualified type names in a C++ `.cc` file into a single
`namespace {  // use_usings` block with `using ::...;`
declarations, and shorten usages in the body accordingly.

**Critical:** There must be exactly **one** use_usings block
per file. Only include symbols under **`::cs::`** (e.g.
`using ::cs::Result;`). Do not add usings for other
top-level namespaces (e.g. `using ::db::Get;`). All usings
must use the **full namespace**. Never add a second block
with short-name usings (e.g. `using ::Result;`). Remove any
redundant second block.

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

Intentions:

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

1. **`::cs` namespace only** – Only include symbols under
   `::cs::` (e.g. `::cs::Result`,
   `::cs::net::http::Request`). Do **not** add symbols from
   other top-level namespaces (e.g. `::db::Get`,
   `::gencode::...`, `::log::...`).
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

## Replacement Rules

- Replace fully-qualified type usages with the short (last)
  segment when:
  - The symbol is type-like.
  - No ambiguity: exactly one fully-qualified symbol maps to
    that short name in the block.
- Do **not** replace:
  - Inside preprocessor lines.
  - Inside string literals or character literals.
  - Inside line or block comments.
  - When followed by `::` (e.g. `SomeType::Nested` stays
    as-is if only `SomeType` is shortened).

## Block Format

```
namespace {  // use_usings
using ::cs::Result;
using ::cs::ResultOr;
using ::cs::net::http::Request;
...
}  // namespace
```

- **Exactly one** block per file; never add a second block.
- **Closing brace:** Use `}  // namespace` (the default);
  never `}  // use_usings`.
- All usings must use the **full namespace** (e.g.
  `using ::cs::Result;`, never `using ::Result;`).
- One `using` per line.
- Sort symbols lexicographically.
- Preserve existing `using` lines that pass the filters;
  merge with newly discovered symbols.

## Block Exclusivity

The `namespace {  // use_usings` block must contain **only**
`using ::...;` declarations. No functions, variables, types,
or other symbols.

If the file has other anonymous-namespace content (helper
functions, static variables, constants, etc.), place them in
a **separate** `namespace { }` block—e.g.
`namespace {  // helpers` or `namespace {  // impl` —not
inside the use_usings block. Do not mix usings with other
declarations in the same block.

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
   that content to a separate anonymous namespace.
4. **Remove redundant blocks:** If the body contains another
   namespace block with only `using ::...;` lines
   (especially short-name usings like `using ::Result;`),
   remove that entire block and merge its symbols (as full
   namespaces) into the main use_usings block.
5. Scan the body for fully-qualified symbols; collect those
   that pass the inclusion rules.
6. Merge with existing usings; filter by visibility. **Drop
   any usings that are not under `::cs::`** (e.g. remove
   `using ::db::Get;`). Ensure all usings use full namespace
   (e.g. `::cs::Result`, not `::Result`).
7. Build the new block and replace fully-qualified usages in
   the body with short names where unambiguous.
8. Write the updated file. Preserve all other content and
   formatting.

## Post-Application Checklist

After applying the protocol, verify:

- **No redundant blocks:** Exactly one
  `namespace {  // use_usings` … `}  // namespace` block
  with usings. No second block with short-name usings.
- **No `using ::Result::Main`:** Use `Result::Main(...)` in
  `main()`; never `using ::Result::Main`.
- **Non-`::cs` symbols:** Never add `using ::db::*`,
  `using ::gencode::*`, `using ::log::*`. Use fully
  qualified names in code (e.g. `db::Get`,
  `::testing::HasSubstr`).
- **Named namespaces:** Code in named namespaces (e.g.
  `cs::apps::...::api`) does not inherit the anonymous
  use_usings block; use fully qualified names or add
  namespace-local usings with full paths.

## Invocation Examples

- `useUsings()` – Process all `.cc` and `.gpt.cc` files in
  the repo.
- `useUsings("path/to/file.cc")` – Process only the given
  file.
- `useUsings("a.cc", "b/b.gpt.cc")` – Process only the given
  files.
