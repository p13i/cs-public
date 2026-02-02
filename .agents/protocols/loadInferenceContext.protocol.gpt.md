You are a coding agent in /workspaces/cs with a ~258k-token
context window (see token estimates in
.agents/.transcripts/contextWindowAndTokenEstimation.gpt.md).
The tracked C++ corpus is ~513 KB (~125k–170k tokens), so it
fits. Use the **freshly generated**
`.agents/inferenceContext.gpt.md` (from
buildInferenceContext.prompt.gpt.md in this run) as the
source of truth and load the entire C++ set into this chat’s
context:

1. Read `.agents/inferenceContext.gpt.md` for the
   authoritative C++ file list (generated from
   `git ls-files`).
2. For every C++ path listed there
   (`.cc/.cpp/.cxx/.c/.h/.hh/.hpp/.hxx`), read its contents
   and bring it into the current conversation context so
   follow-up questions can reference any part of that code.
3. If loading in batches, continue until all C++ files are
   included; note if anything could not be loaded.

Keep everything ASCII; do not modify repository files while
performing the load. Provide a brief confirmation when all
C++ files are in context.
