# Agent instructions for /workspaces/cs

## Core directive

Do exactly what I tell you to do. You will not interpret
meanings beyond the literal text entered into the prompt.

## Language guidelines

- Never use racially-charged terms like "whitelist" or
  "blacklist". Use inclusive alternatives such as
  "allowlist"/"denylist" or "included"/"excluded".
- Avoid the words "enabled"/"disabled"; prefer alternatives
  like "active"/"inactive" or "on"/"off".

## Agent assets

- `.agents/inferenceContext.gpt.md`: Generated snapshot of
  tracked files (SLOC, bytes, tree). Built by running the
  build prompt.
- `.agents/protocols/*.protocol.gpt.md`: Prompt "programs"
  for agents (build/load inference context, dump chat +
  diffs, run continuous integration).
- `.agents/people/p13i/transcripts/*.p13i.gpt.md`: Saved
  chat logs and diffs.
- `.agents/fuzzies/`: Playful/experimental space for
  personas or "imaginary friends."

## Usage notes

- Use
  `.agents/protocols/buildInferenceContext.protocol.gpt.md`
  to regenerate `.agents/inferenceContext.gpt.md`; other
  prompts expect its structure to stay stable.
- `.agents/protocols/runContinuousIntegration.protocol.gpt.md`
  is the only protocol invoked via `make prompt` (wired into
  `.github/workflows/agents.yml`). Do **not** use
  `make prompt` for any other protocol.
- `.agents/protocols/dumpChatContext.protocol.gpt.md` saves
  the current chat and git diff to a new transcript; run
  after other prompts if you want their effects captured.
- All other protocols are executed by the active agent
  reading this file; invoke them directly via the
  `camelCase(...)` message pattern.
- Scope guardrail: do not implement production code or
  standard tests unless the prompt explicitly asks for it;
  if scope is unclear, ask before expanding.
- Do not add new environment variables without explicit user
  approval; prefer config values already in use. If the user
  says not to introduce VARS, inline the provided values and
  avoid new knobs, env vars, or placeholders.
- Do not write documentation in any README.md file unless
  the user explicitly grants permission.
- Avoid ternary operators in code; prefer explicit if/else
  branches.

## GPT-generated file naming rule

- Any file created by GPT or with >80% GPT-generated content
  must include `.gpt` immediately before its extension
  (e.g., `.gpt.py`, `.gpt.bazel`, `.gpt.hh`, `.gpt.yaml`).
  No exceptions.
- Applies to new files and substantive rewrites. If a GPT
  rewrite replaces most of a file, rename it to the `.gpt.*`
  variant before merge.
- Responsibility: the author making the change must apply
  the `.gpt` suffix; reviewers must block merges that
  violate this rule.
- Non-exhaustive examples:
  - Python: `foo.gpt.py`
  - TypeScript/TSX: `widget.gpt.ts`, `widget.gpt.tsx`
  - C++: `core.gpt.cc`, `core.gpt.hh`
  - Bazel/Starlark: `BUILD.gpt.bazel`, `helpers.gpt.bzl`
  - YAML/TOML/JSON: `config.gpt.yaml`, `data.gpt.json`

## Message interpretation priority

Check user messages in this order:

1. **Protocol invocations**: If the message contains a
   `camelCase(...)` pattern (e.g., `planCommits()`,
   `commit()`, `loopUntilExitZero(make)`,
   `loopUntilExitZero("make build", 8)`), treat it as a
   protocol invocation from `.agents/protocols/`. Arguments
   inside parentheses are passed to the protocol. This takes
   precedence over all other rules.
2. **Bash commands**: If the message begins with `$` (and is
   NOT a protocol invocation), run the remainder directly in
   the default bash shell.
3. **Normal agent behavior**: Otherwise, proceed with normal
   agent behavior.

## Agent mode: Plan

When a prompt contains `plan: ...`, create a plan file under
`.agents/plans/` that captures the requested work and stop
there (do not execute the tasks yet). Follow these rules:

- Write a new `.plan.gpt.md` file with a short slug, YAML
  front matter (name, overview, todos), and a Markdown body.
- Mark all todos as pending unless the user explicitly
  states otherwise.
- Do not start implementing the plan until the user asks you
  to execute it or issues non-plan instructions.
- When executing an existing plan, use the `update_plan`
  tool to mark progress one task at a time, starting from
  the first item.
- Plan files live in `.agents/plans`; to-dos are Markdown
  checklist items in the plan document.

## Agent mode: Ask

When a prompt contains `ask: ...`, treat it as a read-only
question. Follow these rules for that prompt:

- Do not run any terminal commands or tools.
- Do not modify files; this is a read-only mode.
- Reply with a concise question or clarification only; do
  not perform other actions or provide extra context beyond
  the question being asked.

## Protocol invocation examples

- `planCommits()` → run
  `.agents/protocols/planCommits.protocol.gpt.md`
- `commit()` → run
  `.agents/protocols/commit.protocol.gpt.md`
- `loopUntilExitZero(make)` → run
  `.agents/protocols/loopUntilExitZero.protocol.gpt.md` with
  command `make`
- `loopUntilExitZero("make build", 8)` → run protocol with
  command `make build` and max iterations `8`
