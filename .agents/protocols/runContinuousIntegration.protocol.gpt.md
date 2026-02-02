You are a coding agent working in /workspaces/cs. Execute
the continuous-integration routine by deterministically
rebuilding `.agents/inferenceContext.gpt.md`, linting, and
reporting status so GPT outputs converge on the same
behavior.

Deterministic runbook (follow in order):

1. Workdir: operate from /workspaces/cs; use repo-relative
   paths.
2. Source of truth: treat
   `.agents/protocols/buildInferenceContext.protocol.gpt.md`
   as the exact spec for generating
   `.agents/inferenceContext.gpt.md` (use `git ls-files`;
   apply its category SLOC/byte rules; render the ASCII
   tree; preserve the precise formatting/structure).
3. Rebuild inference context:
   - Recompute category SLOC/bytes and totals per the build
     prompt definitions.
   - Render the file tree rooted at `.` with the ASCII
     connectors and fenced block exactly as defined.
   - Write `.agents/inferenceContext.gpt.md` in that format
     only; no extra wording or layout drift.
4. Lint:
   - Run `make lint`.
   - If lint rewrites the file, verify the numbers/tree stay
     identical to what you computed; adjust only to restore
     those semantics.
5. Scope control: do not modify other files unless lint
   touches them incidentally; keep everything ASCII.

Report back to the user:

- Brief action summary (e.g., regenerated inferenceContext,
  lint result).
- State whether diffs remain (`git status`/`git diff`
  summary). If blocked, include concise failure details.
