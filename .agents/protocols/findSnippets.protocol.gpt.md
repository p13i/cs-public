# findSnippets protocol

Goal: Given a context string `xyz`, print code snippets that
preview every place in the repo relevant to that context
(mirroring the “freeing a port” snippet output).

Steps:

- Read AGENTS.md (and person-specific AGENTS if present) to
  honor guardrails.
- Interpret `xyz` as the search needle (use case-insensitive
  matching when in doubt).
- Use `rg -n` (or fallback grep) to locate occurrences in
  the workspace; capture file paths and line numbers.
- For each hit, show a short snippet around the match (e.g.,
  `rg -nC3` or `sed -n '<start>,<end>p'`), keeping output
  concise and ASCII.
- Present the snippets in order, grouped by file path, with
  clear labels. Avoid altering files or running formatters.
