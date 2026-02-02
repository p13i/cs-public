# saveThisThread protocol

Goal: write a full, faithful snapshot of the entire current
chat thread—including every user prompt, assistant reply,
assistant/tool thinking steps, and CLI outputs—into a single
Markdown file under the active person’s thread directory
(`.agents/people/<user>/.threads/`). The output must
preserve ordering, separate each message with a line of
`---`, and include clickable links for any code references
(local relative links plus GitHub permalinks). Finish with a
`# Context` section that captures technical details about
the saved context length so the session can be resumed
later. If the protocol is invoked again for the same chat,
rewrite the same thread file (do not create a new slug) so
the transcript stays current as the conversation grows. Do
not call `make prompt` or any other wrapper to run this
protocol—generate and save the transcript directly within
the agent without shelling out.

Steps:

1. Read guardrails
   - Review `AGENTS.md` and
     `.agents/people/<user>/AGENTS.md` to honor existing
     constraints (e.g., no new config variables, no chore:
     prefixes).

2. Pick an output path
   - Use a descriptive slugged filename under
     `.agents/people/<user>/.threads/`, e.g.  
     `.agents/people/<user>/.threads/2025-11-28-some-generated-description-slug.p13i.gpt.md`  
      (keep ASCII; avoid spaces).
   - If a thread file already exists for this chat, reuse
     its path and overwrite it with the updated transcript
     instead of creating a new file.
   - Preflight the target directory before doing anything
     expensive:
     `mkdir -p .agents/people/<user>/.threads/ && test -w .agents/people/<user>/.threads/ || { echo "threads dir not writable; fix ownership or create it first"; exit 1; }`.
     If ownership is root-only or you lack permissions, stop
     immediately and ask for a fix—do not try to sudo.

3. Capture metadata for permalinks
   - `HEAD=$(git rev-parse HEAD)`
   - `REMOTE=$(git remote get-url origin | sed 's#^git@#https://#; s#:#/#')`
   - These feed GitHub permalinks:
     `https://github.com/<org>/<repo>/blob/${HEAD}/<path>#Lx-Ly`.

4. Write the transcript
   - For each message in the current chat context (user,
     assistant, and any tool/CLI outputs or “thinking”
     blocks), emit in order:
     ```
     ---
     Role: User|Assistant|Tool
     <verbatim content>
     ```
   - Do NOT summarize or omit any internal steps; include
     full command outputs shown in the conversation.
   - Keep ASCII; preserve code fences already present.

5. Embed code links
   - Whenever the transcript references a file (e.g.,
     `path/to/file.ext:123`), append two links right after
     the reference:
     - Local:
       `[path/to/file.ext:123](path/to/file.ext#L123)`
     - GitHub:
       `[gh](${REMOTE%/.git}/blob/${HEAD}/path/to/file.ext#L123)`
   - If a line range is known, use `#Lx-Ly` for both.

6. Append diffs (optional but recommended)
   - If the working tree is dirty, append a `## Git Diff`
     section with a fenced `patch` block containing
     `git diff`. Otherwise write “No diffs”.
   - Check dirtiness quickly
     (`git status --porcelain --untracked-files=no`) and
     skip `git diff` entirely if nothing changed to keep the
     run fast.

7. Add `# Context` footer
   - Append a `# Context` section containing technical
     resumption data such as:
     - `context_bytes=<wc -c of transcript>`
     - `message_count=<number of messages saved>`
     - `sha256=<sha256sum of transcript>`
     - Any other machine-readable hints that could help
       recreate the VM/chat state.

8. Save only the transcript file
   - Do not modify other files or run formatters. If writing
     fails, stop and explain.
