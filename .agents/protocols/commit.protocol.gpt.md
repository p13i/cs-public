# commit protocol

Steps for preparing and executing commits in this repo,
reflecting user preferences and prior guardrails.

1. Obey guardrails
   - Read AGENTS.md and the relevant person-specific AGENTS
     file.
   - No commit message prefixes like "chore:"; keep subjects
     plain and descriptive.
   - Follow planCommits protocol when asked for commit() or
     planCommit().

2. Plan first
   - When asked to plan or commit, run the matching protocol
     in .agents/protocols/ (e.g.,
     planCommits.protocol.gpt.md) to produce a granular plan
     with exact SLOC counts per file.
   - Keep commits small and logically grouped (e.g., http
     changes, infra/compose, agent docs, user docs). Do not
     bundle unrelated work.

3. Stage by group
   - Add only the files for the current logical group.
   - Verify with git status before committing.

4. Commit messages
   - Plain, descriptive subject summarizing the group;
     mirror the corresponding planCommits entry so the
     commit title matches the plan output for that grouping.
   - One commit per logical grouping as planned (no
     megacommits).

5. Final checks
   - Ensure the worktree is clean after committing.
   - If a new plan is requested, re-run planCommits after
     changes.

6. If errors happen
   - Prefer soft reset/unstage to correct grouping; do not
     rewrite or amend existing user commits unless
     explicitly requested.
