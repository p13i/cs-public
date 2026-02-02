You are a coding agent working in /workspaces/cs. This
protocol executes a required command in bounded iterations
until it succeeds (exit 0) or the iteration budget is
exhausted, applying autonomous fixes with maximal reasoning
and context from `.agents/inferenceContext.gpt.md`. Do
**not** use shell-level looping constructs (e.g., `until`,
`while`, or `for`) to wrap the command; run it once per
iteration.

Invocation contract (loopUntilExitZero):

- Required: either a concrete command string (e.g.,
  `make build`) or a short prose description of the desired
  end state (e.g., “service is reachable and returns HTML”).
  A prose goal alone is considered valid input; do not
  request an explicit command if one is not provided.
- If prose is provided, derive and run the concrete commands
  needed to reach/verify that end state, narrating each
  attempt, and treat the end-state verification as the
  success condition.
- If nothing actionable is provided, stop and request a
  concrete command or an end-state description.
- Optional: iteration count (integer). Default 4; clamp to
  [1, 32]. If the provided value is outside bounds or
  non-integer, stop and tell the user to resubmit within the
  range.

Operational runbook:

1. Workdir: operate from /workspaces/cs; honor AGENTS.md and
   related repo instructions. Keep everything ASCII.
2. Load context: consult `.agents/inferenceContext.gpt.md`
   for relevant files and signals to guide fixes; use the
   fullest applicable context window.
3. Loop i=1..max_iterations:
   - If input was prose, determine the next concrete
     command(s) that move toward or verify the described end
     state.
   - Execute the command in a shell; capture exit code plus
     stdout/stderr.
   - Never wrap the command in an `until`/`while`/`for`
     shell loop; each iteration runs the command once.
   - If exit code is 0:
     - Summarize the successful run.
     - Stage repo changes (`git add -A`) and report
       `git status -sb`/`git diff --stat` summaries.
     - Stop looping and inform the user.
   - If exit code is non-zero:
     - Diagnose root causes using the inference context and
       codebase knowledge.
     - Apply autonomous, expert-level code/config edits
       (highest reasoning/model capacity available) to fix
       the issue before the next attempt. Avoid reverting
       unrelated changes; keep scope relevant.
     - Re-run the command on the next iteration (single run
       only) after applying fixes.
     - Continue to the next iteration with the same command.
4. If all iterations fail to reach exit 0:
   - Report the failure, remaining diffs, and next steps to
     the user.

Reporting expectations:

- Always relay iteration outcomes, what changed, whether
  changes are staged, and remaining diffs.
- If blocked by invalid inputs, state the guard that fired
  and stop without edits.
