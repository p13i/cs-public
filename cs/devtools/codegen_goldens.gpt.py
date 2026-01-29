#!/usr/bin/env python3
# cs/devtools/codegen_goldens.gpt.py
# Regenerate codegen goldens for cs/net/proto/codegen.

from __future__ import annotations

import shutil
import tempfile
import importlib.util
from pathlib import Path


def _load_golden_utils():
    module_path = (
        Path(__file__).resolve().parent.parent
        / "net/proto/codegen/tests/golden_utils.py"
    )
    spec = importlib.util.spec_from_file_location("golden_utils", module_path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module


golden_utils = _load_golden_utils()
GOLDEN_REPO_ROOT = golden_utils.REPO_ROOT
copy_fixtures = golden_utils.copy_fixtures
run_codegen_single = golden_utils.run_codegen_single


def main() -> None:
    golden_root = GOLDEN_REPO_ROOT / "cs/net/proto/codegen/testdata/golden"
    workspace_root = Path(tempfile.mkdtemp(prefix="codegen_goldens_workspace_"))
    try:
        copy_fixtures(workspace_root)
        run_codegen_single(
            workspace_root, "cs/test/protos/validation.proto.hh", golden_root
        )
        run_codegen_single(
            workspace_root, "cs/another/protos/simple.proto.hh", golden_root
        )
        print("[codegen-goldens] regenerated goldens under", golden_root)
    finally:
        shutil.rmtree(workspace_root, ignore_errors=True)


if __name__ == "__main__":
    main()
