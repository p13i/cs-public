# newFile(protocol) for templated files

Goal: create a new file from the repo template generator by
running `make newfile` with a required relative path.

Invocation contract (newFile(requiredRelativePath)):

- Required: `requiredRelativePath` — path relative to repo
  root (e.g., `cs/q/trees/node.hh`). If missing, empty, or
  absolute, stop and ask for a valid relative path.

Steps:

1. Guardrails: read `AGENTS.md` plus any person-specific
   notes; keep edits ASCII; work from repo root.
2. Ensure the parent directory exists:
   `mkdir -p "$(dirname "$requiredRelativePath")"`.
3. Run the generator:
   `NEWFILE=$requiredRelativePath make newfile`.
4. Verify the file was created with the expected content
   (e.g., `sed -n '1,40p' "$requiredRelativePath"`); report
   the result and any errors. Do not stage unless asked.
