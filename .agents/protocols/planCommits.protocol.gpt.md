# planCommits protocol

Actionable steps for an agent to produce a granular commit
plan with exact SLOC counts and per-file summaries, suitable
for line-level staging and easy reverts.

1. Read context
   - Load AGENTS.md and AGENTS.p13i.md guardrails.
   - Inspect git status to see modified/untracked files.
   - Note the user’s formatting/commit-style preferences
     (e.g., no prefixes, exact SLOC counts, summary
     placement).

2. Collect diffs and counts
   - Run `git diff --numstat` to capture added/deleted line
     counts per file.
   - For new files, use `wc -l <file>` to get line counts;
     for gitlinks/submodules, record as 0 SLOCS added and
     note gitlink.
   - Do not use approximate (“~”) counts; prefer exact
     numbers.

3. Group logical commits
   - Identify logical groupings (e.g., submodule add, config
     changes, Makefile changes, documentation,
     lint/format-only).
   - Avoid mixing unrelated changes; aim for small,
     revert-friendly commits.

4. Per-file detail
   - For each file in a grouping, provide:
     - Summary (what changed, in plain language).
     - SLOCS added/updated/deleted (exact counts from
       numstat; if numstat shows only add/del, set
       updated=0).
   - Treat format-only changes as a separate grouping.

5. Render plan
   - Output as a numbered list (one entry per proposed
     commit).
   - Under each entry, use sub-bullets per file; within each
     file, include two sub-bullets: Summary, SLOCS
     added/updated/deleted.
   - Keep ASCII, concise phrasing, and adhere to user’s
     layout preferences.

6. Persist if requested
   - If asked to save the plan, write it to the specified
     path (e.g., an .agents/.transcripts file), preserving
     the structure above.
   - Do not alter unrelated files.

7. Execute (when instructed)
   - Follow the plan: stage by grouping (use `git add -p` if
     needed), commit with user-approved style, and verify
     clean status.
   - Report results succinctly.
