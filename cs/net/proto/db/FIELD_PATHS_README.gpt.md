# Field Path Resolution for Query Helpers

## Overview

This directory contains support for type-safe field path
resolution in database queries. Field paths can be specified
using either:

1. **String literals**: `"email"`
2. **Field references**: `&User::email`

Both approaches resolve to the same string value, allowing
for flexible query construction.

## Files Created

### Core Infrastructure

- **`field_path_builder.gpt.hh`**: Template infrastructure
  for resolving field paths
  - `GetFieldPath<T>()`: Template function (requires
    specialization per proto)
  - `GetFieldPathOrConvert()`: Universal converter
    supporting strings and field references
  - `FieldPathBuilder<T>`: Builder for nested field paths
    (future use)

- **`query_helpers.gpt.hh`**: Query helper functions that
  use field path resolution
  - `EQUALS()`, `CONTAINS()`, `NOT_CONTAINS()` functions
  - Accepts both string literals and field references

### Codegen Support

- **`cs/apps/trycopilot.ai/protos/gencode/user.field_paths.gpt.hh`**:
  Generated specializations for User proto
  - Specializes `GetFieldPath()` for all User fields
  - Specializes `GetFieldPath()` for all Password fields
  - Maps field member pointers to their string names

- **`cs/apps/trycopilot.ai/protos/gencode/BUILD`**: Build
  target for field path codegen

### Tests

- **`test_field_paths.gpt.cc`**: GTest unit tests
  - Verifies string literals work
  - Verifies field references work
  - Verifies both approaches produce identical results
  - Contains 11 comprehensive test cases
  - **Note**: Bazel target currently blocked by toolchain
    issues (emsdk)

## Usage Example

```cpp
using cs::net::proto::db::EQUALS;
using User = cs::apps::trycopilotai::protos::User;

// Both of these produce identical queries:

// Approach 1: String literal (backward compatible)
users_query.where(EQUALS("email", request.email))

// Approach 2: Field reference (type-safe)
users_query.where(EQUALS(&User::email, request.email))
```

## Running Tests

### Bazel Test (Recommended)

```bash
bazel test //cs/net/proto/db:test_field_paths
```

**Current Issue**: Bazel 9.0.0 has toolchain resolution
errors with emsdk 4.0.17. The
emscripten_toolchain/toolchain.bzl fails to initialize. This
needs to be resolved separately from the field path
functionality.

## Future Work

1. **Fix Bazel Toolchain Issues**: Resolve emsdk
   compatibility with Bazel 9.0.0
2. **Codegen Automation**: Create a tool to generate
   field_paths.gpt.hh files automatically from proto
   definitions
3. **Nested Field Paths**: Extend to support paths like
   `&User::password >> &Password::hash` for nested queries
4. **Additional Protos**: Generate field_paths for Token,
   LoginRequest, and other protos as needed

## Why Field References?

Field references provide several advantages over string
literals:

1. **Type Safety**: Compiler catches typos and missing
   fields
2. **Refactoring Support**: Renaming fields updates all
   references automatically
3. **IDE Support**: Autocomplete, go-to-definition,
   find-all-references
4. **Self-Documenting**: Makes it clear which proto type is
   being queried
5. **Backward Compatible**: String literals still work via
   GetFieldPathOrConvert()
