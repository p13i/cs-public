# newLab(protocol) for hello-world labs

Goal: add a minimal, tested Hello World lab for a given
programmingLanguage under `cs/labs/<lang>/` so that `make`,
`make test`, and `make lint` keep working in the
devcontainer/WSL/Bazel context. Tests must be runnable via
`bazel test //cs/...` with no extra user steps.

Steps:

1. Guardrails
   - Read `AGENTS.md` plus any person-specific
     `.agents/people/<user>/AGENTS.md`.
   - Keep edits ASCII; avoid touching unrelated files.

2. Create lab skeleton
   - New dir: `cs/labs/<lang>/`.
   - Add `acl.json` (public allow-all).
   - Add source (Hello World). Keep lines ≤60 chars; include
     `.editorconfig` if helpful for the language.
   - Add a Bazel test that **imports the lab source as a
     library/module** (not just running a binary) to prove
     intra-lab imports work. Preferred: language-native test
     rule when available; otherwise `sh_test` that copies
     sources to a temp dir, runs the program via the
     language CLI, asserts output, and calls the width
     check.
   - Include a width-check helper that enforces the 60-col
     rule on all files in the lab; call it from the test so
     Bazel enforces lint even without Makefile changes.

3. Tooling/runtime
   - If the language needs extra tooling, add it to both
     `Dockerfile` and `make setup-dockerfile` (e.g., apt
     installs) so devcontainer/WSL users get the same bits
     and `make test` passes.
   - Suppress first-run banners/telemetry and separate CLI
     flags from app args (e.g.,
     `DOTNET_CLI_TELEMETRY_OPTOUT=1`,
     `DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1`,
     `DOTNET_NOLOGO=1`, and `dotnet run ... --`) so test
     output stays stable.
   - For VS Code debugging, add tasks/launch entries under
     `.devcontainer/` using the language’s debugger
     extension; do not break existing entries.

4. Lint wiring
   - If the lab needs a Makefile hook, add a `lint-<lang>`
     target that calls the width-check helper and include it
     in the `lint` aggregate. Do not alter existing behavior
     of `make`, `make build`, docker-compose targets, etc.

5. Verify
   - Run the lab-specific test:
     `bazel test //cs/labs/<lang>:hello_world_test`.
   - Run `make lint` if you touched lint wiring.
   - Confirm line-width check passes.
   - If the test fails, run the loopUntilExitZero protocol
     with the same bazel test command to iterate fixes until
     exit 0, then stage changes.

6. Document
   - In your summary, note dependencies added (e.g., SDK
     version) and how to run the new test (`make test`
     already covers it).
7. Commit small
   - Commit in very small, vertical slices (even sub-file
     hunks) so bisects stay precise; keep toolchain, lint,
     tests, and IDE configs separate.
