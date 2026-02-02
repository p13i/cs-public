# Development with Bots: Reflections on Prompt-Only Fixing

A case study: fixing the login flow and `GetFieldPath`
member-pointer invariant **without editing a single line of
code**—using only prompts and protocol invocations.

---

## Summary

The login flow was broken due to `GetFieldPath` overload
resolution: string paths were a workaround, but the goal was
member pointers only. Through prompts, protocols, and
iterative feedback, the fix was implemented entirely by the
AI agent. This document captures what made that approach
work.

---

## What Made the Prompt-Only Fix Work

### 1. Structured Protocol Invocations Over Open-Ended Prompts

Instead of "please fix my login," the workflow used
protocol-style calls:

- `planCommits()` / `commit(planCommits(granular))` —
  planned and executed commits
- `versionUp(patch)` — bumped version and tagged
- `useUsings(cs/apps/trycopilot.ai/api/logs.cc)` —
  consolidated usings in a file
- `loopUntilExitZero(make b t)` — build and test until green

These protocols are defined in `.agents/protocols/` with
clear steps. The model could follow them instead of
inventing a workflow.

### 2. Explicit Guardrails in AGENTS.md

`AGENTS.md` provided rules the agent had to obey:

- No commit prefixes (e.g. no "chore:")
- Language guidelines (no "whitelist"/"blacklist", prefer
  "allowlist"/"denylist")
- GPT-generated file naming (`.gpt` before extension)
- Scope limits (no production code unless explicitly asked)
- Message interpretation order (protocols > bash > normal
  behavior)

That created predictable behavior and reduced ambiguity.

### 3. Small, Focused Prompts

Work was broken into narrow, actionable steps:

- "Add the include that brings access to the GetFieldPath
  specifier"
- "Make a system invariant against missing GetFieldPath
  specializations"
- "Display an error message before/in std::abort"
- "Use member pointers, not strings. Do NOT support string
  inputs"
- "Write up those tests" (for register/login/logout flows)
- "UN DO" (when .field = value syntax was wrong)

Each prompt had one main goal, which made the model’s
changes targeted.

### 4. Sticking to the Right Abstraction

When linker errors made string literals appealing, the user
insisted on member pointers. That forced fixing the
underlying issue (ADL, proto namespace, explicit
instantiations) instead of patching around it with strings.

### 5. Test-Driven Feedback Loop

Specific flows were requested and turned into tests:

- register → home → logout → index
- login → home → logout → index (existing user)
- `/home/` must return 403 or 404 after logout

Then: `loopUntilExitZero(make b t)`. Test failures (e.g.
`GetFieldPath: no specialization for type`) gave concrete
error messages that could be turned into prompts.

### 6. Existing Structure and Conventions

The codebase had clear patterns:

- Gencode for protos and `GetFieldPath` specializations
- `EQUALS`, `QueryView`, `FieldPathBuilder`
- Proto namespaces and build layout

The model could infer how to change things within these
conventions.

### 7. Explicit Correction When Wrong

When the model added `.field = value` syntax that wasn’t
wanted, the user said "UN DO." That kept the model from
continuing in the wrong direction and made the desired
behavior unambiguous.

---

## Technical Outcome

The fix involved:

1. Moving `GetFieldPath` into each proto’s namespace so ADL
   selects the right overload
2. Adding `if constexpr` for type-safe member-pointer
   comparisons
3. Emitting explicit template instantiations in proto `.cc`
   files
4. Removing string support from `GetFieldPathOrConvert`
5. Updating auth, user, ui, and logs to use member pointers
   only
6. Adding integration tests for the login/logout flows

---

## Takeaways for Future Sessions

1. **Use protocols** — Invoke `planCommits()`, `commit()`,
   `versionUp()`, etc. instead of describing workflows in
   prose.
2. **Keep prompts small** — One focused request per prompt
   where possible.
3. **Enforce abstractions** — Don’t relax constraints (e.g.
   to strings) when the real fix is harder.
4. **Test early** — Ask for tests and run them
   (`loopUntilExitZero`) to get fast, concrete feedback.
5. **Correct directly** — Use "UN DO" or similar when the
   model goes off-track.
6. **Keep guardrails up to date** — `AGENTS.md` and
   protocols shape behavior; update them as you learn.

---

## Models Used

This report does not track which specific models handled
which prompts. Cursor session and usage logs would show
that. Typical usage involves a chat model for conversation
and an agent model for tool use (edits, terminals,
protocols).
