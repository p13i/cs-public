# syncBazelDeps protocol

Goal: ensure every tracked C++ source/header under `cs/` has
a direct Bazel dep set that matches the `cs/...` includes it
uses (one-to-one), without touching non-`cs` deps.

Invocation contract (syncBazelDeps()):

- No arguments; scope is fixed to tracked files in `cs/`.

Runbook:

1. Guardrails: read AGENTS.md + person-specific notes; work
   from repo root; keep edits ASCII; avoid reverting user
   changes.
2. Collect Bazel cc targets with macros resolved:
   - Set
     `BAZEL="USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk"`.
   - Run
     `${BAZEL} query "kind('cc_(library|binary|test)', //cs/...)" --output=build > /tmp/cs_cc_targets.bzl`.
   - Parse each `cc_*` block to capture package, rule kind,
     deps/srcs/hdrs (collect all label strings inside lists
     or select()), plus generator_function/location when
     present. Build:
     - target_info[label] = {kind, pkg, deps, buildfile,
       generator_function?, generator_location?}
     - owner_map[file_path] -> list of provider labels,
       where file_path is `pkg/relpath` from labels like
       `//pkg:relpath`.
3. Scan includes from sources:
   - Files = `git ls-files "cs/**/*.cc" "cs/**/*.hh"` (also
     include `"cs/**/*.h"` because this repo uses `.h` for
     C++ headers).
   - For each file, find owning targets via owner_map; note
     any missing owners.
   - Extract include paths matching
     `#include ["<](cs/[^">]+)[">]`.
   - Normalize includes to `cs/...` paths; if not in
     owner_map, record for manual fix.
   - Map each include to a provider target: prefer
     cc_library, then cc_binary, then cc_test; break ties
     lexicographically. Skip provider==consumer. Accumulate
     needed_cs_deps[consumer_target].
4. Compute expected deps per target:
   - existing_cs_deps = deps in target_info starting with
     `//cs`.
   - needed_cs_deps = union from step 3.
   - additions = needed_cs_deps - existing_cs_deps; removals
     = existing_cs_deps - needed_cs_deps.
   - Keep non-`cs` deps untouched; dedup and sort cs deps
     for stable output.
5. Rewrite deps:
   - For direct cc\_\* rules, edit that package’s BUILD to
     set `deps = [` non-`cs` deps (original order) + sorted
     cs deps `]`, rendering same-package deps as `:name`.
   - For generator-based rules (generator_function present),
     update the macro invocation at generator_location (not
     the generated rule) by supplying a `deps = [...]`
     argument that yields the needed set (account for macro
     defaults; avoid duplicates).
   - Add a deps attr if absent; remove unused cs deps; one
     dep per line with trailing commas.
6. Format and validate:
   - Run `buildifier` on touched BUILD files (or `make lint`
     if already expected).
   - Optionally `${BAZEL} build --nobuild <changed targets>`
     to ensure dependency graph resolves.
7. Reporting:
   - Summarize targets changed, cs deps added/removed, and
     any unresolved includes/owners.
   - Share `git status -sb` and `git diff --stat`; call out
     skipped steps or follow-ups.
