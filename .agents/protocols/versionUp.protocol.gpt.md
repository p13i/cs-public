# versionUp protocol

Goal: bump the display `VERSION` in `.env` (semantic triad),
sync `COMMIT` to the latest HEAD, then commit and tag the
resulting version.

Invocation: `versionUp(major|minor|patch)` Default is
`patch` if no argument is provided. If provided, validate it
is one of `major`, `minor`, or `patch`; stop otherwise.

Steps:

- Read AGENTS.md and person-specific AGENTS to honor
  guardrails.
- Use the argument if provided; otherwise use `patch`.
  Validate it is one of `major`, `minor`, or `patch`; stop
  otherwise.
- Ensure the working tree is clean except for `.env` (or
  note/avoid staging other files).
- Read `VERSION` from `.env`; accept optional leading `v`
  and numeric `X.Y.Z` (default to `0.0.1` if absent).
  Increment:
  - major: X+1, reset Y=0, Z=0
  - minor: Y+1, reset Z=0
  - patch: Z+1 Preserve the leading `v` if present.
- Set `COMMIT` to `git rev-parse --short=7 HEAD`.
- Rewrite `.env` with updated `VERSION` and `COMMIT`,
  preserving other keys/lines (sorted KVs are ok).
- Show the new `.env` values for `VERSION` and `COMMIT`.
- Stage only `.env`.
- Commit with subject: `$VERSION ($COMMIT)` (substitute the
  actual values after updating).
- Create a tag named exactly the new `VERSION` string (keep
  the leading `v` if present) pointing at that commit; abort
  if the tag already exists.
- Verify the working tree is clean.

Helper script (python):

Use this to read, update, and rewrite `.env` in one step.
Pass the bump type as the first argument
(`major|minor|patch`); default is `patch` if omitted.

```
BUMP=patch
python - "$BUMP" <<'PY'
import re
import subprocess
import sys
from pathlib import Path

bump = sys.argv[1] if len(sys.argv) > 1 else "patch"
if bump not in {"major", "minor", "patch"}:
    raise SystemExit("usage: python - <major|minor|patch> <<'PY'")

env_path = Path(".env")
lines = env_path.read_text(encoding="utf-8").splitlines()

version_index = None
commit_index = None
raw_version = None
for idx, line in enumerate(lines):
    if line.startswith("VERSION="):
        version_index = idx
        raw_version = line.split("=", 1)[1].strip()
    elif line.startswith("COMMIT="):
        commit_index = idx

version = raw_version or "0.0.1"
version = version.strip('"').strip("'")
match = re.match(r"^(v)?(\d+)\.(\d+)\.(\d+)$", version)
if not match:
    raise SystemExit(f"invalid VERSION: {version!r}")

prefix, major, minor, patch = match.group(1) or "", int(match.group(2)), int(match.group(3)), int(match.group(4))
if bump == "major":
    major += 1
    minor = 0
    patch = 0
elif bump == "minor":
    minor += 1
    patch = 0
else:
    patch += 1

new_version = f"{prefix}{major}.{minor}.{patch}"
commit = subprocess.check_output(
    ["git", "rev-parse", "--short=7", "HEAD"], text=True
).strip()

version_line = f"VERSION={new_version}"
commit_line = f"COMMIT={commit}"

if version_index is None:
    lines.append(version_line)
else:
    lines[version_index] = version_line

if commit_index is None:
    lines.append(commit_line)
else:
    lines[commit_index] = commit_line

env_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
print(f"VERSION={new_version}")
print(f"COMMIT={commit}")
PY
```
