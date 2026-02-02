# purge(protocol) for removing paths across history

Input args (required):

- `paths`: one or more repo-relative paths to remove (e.g.,
  `cs/q/count_bit_sequences`).

Goal: remove the specified paths from a rewrite clone (e.g.,
`../danger-close/cs-rewrite`) without pushing to any remote.

## Preconditions

- Freeze merges to `main` on the authoritative remote.
- Ensure `git-filter-repo` is available (venv install is
  fine).
- Backup: Keep the original repo untouched. Tag current
  `main` in the rewrite clone for reference (e.g.,
  `git tag pre-purge-count_bit_sequences`).

## Rewrite Steps (rewrite clone)

1. From the rewrite clone root: record `git rev-parse HEAD`
   and ensure clean status.
2. Run filter (repeat `--path` for multiple targets):  
   `git filter-repo --path <path1> [--path <pathN> ...] --invert-paths --prune-empty always --force`  
   (If using a venv install, call via
   `./.venv/bin/git-filter-repo`.)
3. Inspect filter report; sanity-check logs:
   `git log --stat | head -n 40`.
4. Verify absence:
   - `git log --all -- <path>` (expect none)
   - `git grep -n --no-index '<basename>'` for the removed
     paths.
5. Optional archive sanity check:
   `git archive HEAD | tar -t | grep <basename>` (expect no
   output).

## Post-rewrite Safety

- Do **not** push. Remote protections stay in place. Note:
  `git filter-repo` drops `origin`; re-add only if
  coordinated.
- If a push is ever required, force-push all refs
  together:  
  `git push --force --all origin` and
  `git push --force --tags origin` (not performed here).

## Cleanup Guidance for Collaborators (if rewrite is adopted)

- Require reclones or
  `git fetch --all && git reset --hard origin/main` after
  deleting local branches tied to old history.
- Invalidate CI caches and artifacts that might embed the
  removed paths; regenerate SBOM/provenance if maintained.

## Rollback

- Use the saved tag (e.g., `pre-purge-count_bit_sequences`)
  in the rewrite clone to recover prior history if needed.
  Keep an offline copy of the original repository for
  diffing.
