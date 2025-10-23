## Migration of `rules_rust`:

<details>
<summary>Click here to see where and how the repo was declared in the WORKSPACE file</summary>

#### Location

```python
Repository rules_rust instantiated at:
  /home/pramo/cs/WORKSPACE:33:13: in <toplevel>
Repository rule http_archive defined at:
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>

```

#### Definition

```python
load("@@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
  name = "rules_rust",
  urls = [
    "https://github.com/bazelbuild/rules_rust/releases/download/0.48.0/rules_rust-v0.48.0.tar.gz"
  ],
  integrity = "sha256-Weev1uz2QztBlDA88JX6A1N72SucD1V8lBsaliM0TTg=",
)
```

**Tip**: URLs usually show which version was used.

</details>

---

Found perfect name match in BCR: `rules_rust`

Found partially name matches in BCR: `rules_rust_mdbook`,
`rules_rust_prost`, `rules_rust_bindgen`,
`rules_rust_protobuf`, `rules_rust_wasm_bindgen`

It has been introduced as a Bazel module:

    bazel_dep(name = "rules_rust", version = "0.65.0")

## Migration of `emsdk`:

<details>
<summary>Click here to see where and how the repo was declared in the WORKSPACE file</summary>

#### Location

```python
Repository emsdk instantiated at:
  /home/pramo/cs/WORKSPACE:10:15: in <toplevel>
Repository rule git_repository defined at:
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/git.bzl:189:33: in <toplevel>

```

#### Definition

```python
load("@@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
git_repository(
  name = "emsdk",
  remote = "https://github.com/emscripten-core/emsdk.git",
  commit = "a896e3d066448b3530dbcaa48869fafefd738f57",
  strip_prefix = "bazel",
)
```

**Tip**: URLs usually show which version was used.

</details>

---

Found perfect name match in BCR: `emsdk`

It has been introduced as a Bazel module:

    bazel_dep(name = "emsdk", version = "4.0.16")

## Migration of `rules_cc`:

<details>
<summary>Click here to see where and how the repo was declared in the WORKSPACE file</summary>

#### Location

```python
Repository rules_cc instantiated at:
  /home/pramo/cs/WORKSPACE:41:24: in <toplevel>
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:60:10: in rules_rust_dependencies
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe
Repository rule http_archive defined at:
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>

```

#### Definition

```python
load("@@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
  name = "rules_cc",
  urls = [
    "https://github.com/bazelbuild/rules_cc/releases/download/0.0.9/rules_cc-0.0.9.tar.gz"
  ],
  sha256 = "2037875b9a4456dce4a79d112a8ae885bbc4aad968e6587dca6e64f3a0900cdf",
  strip_prefix = "rules_cc-0.0.9",
)
```

**Tip**: URLs usually show which version was used.

</details>

---

Found perfect name match in BCR: `rules_cc`

It has been introduced as a Bazel module:

    bazel_dep(name = "rules_cc", version = "0.2.9")

## Migration of `rules_python`:

<details>
<summary>Click here to see where and how the repo was declared in the WORKSPACE file</summary>

#### Location

```python
Repository rules_python instantiated at:
  /home/pramo/cs/WORKSPACE:51:13: in <toplevel>
Repository rule http_archive defined at:
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>

```

#### Definition

```python
load("@@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
  name = "rules_python",
  url = "https://github.com/bazel-contrib/rules_python/releases/download/0.36.0/rules_python-0.36.0.tar.gz",
  sha256 = "ca77768989a7f311186a29747e3e95c936a41dffac779aff6b443db22290d913",
  strip_prefix = "rules_python-0.36.0",
)
```

**Tip**: URLs usually show which version was used.

</details>

---

Found perfect name match in BCR: `rules_python`

It has been introduced as a Bazel module:

    bazel_dep(name = "rules_python", version = "1.6.3")

## Migration of `multiset_deps`

It has been introduced as a python extension:

```
pip.parse(
    hub_name = "multiset_deps",
    requirements_lock = "//cs/q/multiset:requirements.txt",
    python_version = "3.11",
)
use_repo(pip, "multiset_deps")

python = use_extension("@rules_python//python/extensions:python.bzl", "python")
python.defaults(python_version = "3.11")
python.toolchain(python_version = "3.11")
```

## Migration of `googletest`:

<details>
<summary>Click here to see where and how the repo was declared in the WORKSPACE file</summary>

#### Location

```python
Repository googletest instantiated at:
  /home/pramo/cs/WORKSPACE:3:19: in <toplevel>
Repository rule git_repository defined at:
  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/git.bzl:189:33: in <toplevel>

```

#### Definition

```python
load("@@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
git_repository(
  name = "googletest",
  remote = "https://github.com/google/googletest",
  commit = "52eb8108c5bdec04579160ae17225d66034bd723",
  build_file = "//:gmock.BUILD",
)
```

**Tip**: URLs usually show which version was used.

</details>

---

Found perfect name match in BCR: `googletest`

It has been introduced as a Bazel module:

    bazel_dep(name = "googletest", version = "1.17.0.bcr.1")
