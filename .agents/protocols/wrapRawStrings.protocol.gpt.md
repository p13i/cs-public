# wrapRawStrings protocol

Goal: wrap C++ raw string literals so every line of the
source stays within the project line-length limit, with even
breaks at word boundaries (no jagged edges, no mid-word
cuts).

Invocation: `wrapRawStrings()` or `wrapRawStrings(path)` or
`wrapRawStrings(path1, path2, ...)`. With no arguments, the
agent may prompt for path(s) or infer from context (e.g.
file under discussion). Paths are relative to the repo root;
only C++ sources (e.g. `.cc`, `.hh`, `.gpt.cc`, `.gpt.hh`)
are in scope.

Steps:

- Read AGENTS.md and honor guardrails (e.g. no new env vars,
  prefer existing config).
- Resolve line-length limit: read `.clang-format` and use
  `ColumnLimit` (default 60 if missing). Call this `L`.
- For each target file:
  - Locate raw string literals: `R"( ... )"` and
    `R"delim( ... )delim"` (including multi-line).
  - Identify lines that exceed `L` characters (including
    indentation). Focus on the content lines inside the raw
    string (continuation lines).
  - Recompute breaks so that:
    - Each source line has length ≤ `L`.
    - Continuation lines use a single multi-line raw string
      (one `R"(` ... `)"`), not concatenated `R"()"` chunks.
    - Break only at word boundaries (spaces); never split a
      word (e.g. avoid "engi-\nneering" or "wor" at end of
      line).
    - Aim for even right margin: choose break points so
      continuation lines have similar content length (e.g.
      if indent is 16 spaces and `L` is 60, aim for ~40–44
      characters of content per continuation line).
  - Rewrite the file with the new line breaks; preserve all
    other formatting and logic.
- Do not change wording or fix typos inside the strings
  unless the user asks; only adjust line breaks.

Intent (summary):

- One logical string = one raw string literal that may span
  multiple lines (newlines and spaces inside the literal are
  part of the string).
- Column limit from `.clang-format` is the single source of
  truth for line length.
- Breaks are for readability and lint compliance; the visual
  result should look even, not jagged.
