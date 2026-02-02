You are a coding agent working in /workspaces/cs. This
prompt is the **authoritative generator** for
`.agents/inferenceContext.gpt.md`; downstream prompts (e.g.,
loadInferenceContext) rely on its structure staying stable.
Produce a fresh `.agents/inferenceContext.gpt.md` from
scratch. Respect .gitignore by using `git ls-files` for all
discovery so only tracked, non-ignored files are included.

Counting rules (apply to tracked files only):

- Categories: Python (.py); C++
  (.cc/.cpp/.cxx/.c/.h/.hh/.hpp/.hxx); Bazel (.bzl/.bazel
  plus BUILD/WORKSPACE and their .bazel variants); Makefile
  (Makefile/makefile/GNUmakefile/.mk); acl.json (exact
  filename `acl.json`).
- SLOC: count non-empty lines (trim with `strip()` and
  include the line if non-empty) using UTF-8 with
  `errors='ignore'`.
- Bytes: filesystem byte size via `stat().st_size`.
- Totals: sum SLOC, file counts, and bytes across
  categories.

File tree rules:

- Build a tree from the `git ls-files` list, sorted
  lexicographically, with directories before files at each
  level.
- Render using ASCII connectors `|--` and ``--`; indent
  child entries with `| ` when more siblings follow, or four
  spaces when it is the last child.
- Root the tree at `.` and wrap the entire tree in a fenced
  code block (triple backticks) to prevent wrapping by
  formatters.

Output format for `.agents/inferenceContext.gpt.md`:

1. Intro paragraph describing the counting method and
   category definitions (as in this prompt, single
   paragraph).
2. Bullet list per category: “- {Category}: {SLOC} SLOC
   across {N} files” using the freshly computed numbers.
3. Markdown table with columns Category, SLOC, Files, Bytes,
   plus a bold Total row containing the summed values.
4. “File tree (tracked via git ls-files):” header followed
   by the fenced ASCII tree described above.
5. Keep everything ASCII.

After writing the file, run `make lint` (runs
lint-newfile-examples, lint-makefile, filenames_in_comments,
inline_using, check_visibility, enforce_header_guards,
buildifier, clang-format, prettier on md/html/yml/yaml/
json, prettier-plugin-rust, black via Bazel). It may
reformat the output; the final
`.agents/inferenceContext.gpt.md` content after linting
should still match the structure and counts you computed. If
linting changes the file, keep the semantics identical and
do not tweak the layout beyond what lint enforces.
