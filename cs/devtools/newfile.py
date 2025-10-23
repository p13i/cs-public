"""
Created by ChatGPT:
https://chatgpt.com/share/67dde23d-99d0-800e-8eb9-26de6a62c124
"""

import os
import sys


def generate_header_guard(file_path: str) -> str:
    """Generate a C++ header guard based on the file path."""
    guard = file_path.replace(os.sep, "_").replace(".", "_").upper()
    return f"#ifndef {guard}\n#define {guard}\n"


def generate_namespace(file_path: str) -> str:
    """Generate a C++ namespace based on the file path."""
    parts = file_path.split(os.sep)[:-1]  # Exclude the filename itself
    namespace = "::".join(parts)
    return f"namespace {namespace}"


def create_header_file(repo_dir: str, file_path: str):
    """Create a C++ header file with header guards and namespace."""
    guard = generate_header_guard(file_path)
    namespace = generate_namespace(file_path)

    content = f"""
{guard}
{namespace} {{

}}  // {namespace}

#endif  // {guard.split()[1]}
""".strip()

    with open(os.path.join(repo_dir, file_path), "w") as f:
        print(f"Writing content={content} to file_path={file_path}")
        f.write(content)


def create_source_file(repo_dir: str, file_path: str, header_path: str):
    """Create a C++ source file that includes the corresponding header and defines a namespace."""
    namespace = generate_namespace(file_path)
    include_guard = f'#include "{header_path}"'

    content = f"""
{include_guard}

{namespace} {{

}}  // {namespace}
""".strip()

    with open(os.path.join(repo_dir, file_path), "w") as f:
        f.write(content)


def create_test_file(repo_dir: str, file_path: str, header_path: str):
    content = f"""
#include "{header_path}"

#include <stdio.h>

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {{  // use_usings
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::FieldsAre;
using ::testing::FloatEq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Matcher;
using ::testing::Not;
using ::testing::StrEq;
}}  // namespace

namespace {{
// TEST
}}  // namespace
"""
    with open(os.path.join(repo_dir, file_path), "w") as f:
        f.write(content)


def main():
    """Main function to handle file creation."""
    if len(sys.argv) < 3:
        print("Usage: script.py <repo_dir> <file_path>")
        print(sys.argv)
        return

    repo_dir = sys.argv[1]
    file_path = sys.argv[2]

    print(f"file_path={file_path}")

    if not os.path.exists(os.path.dirname(file_path)):
        os.makedirs(os.path.dirname(file_path))

    if file_path.endswith(".hh"):
        print("Generating header file...")
        create_header_file(repo_dir, file_path)
    elif file_path.endswith("test.cc"):
        print("Generating test file...")
        create_test_file(
            repo_dir, file_path, header_path=file_path.replace("_test.cc", ".hh")
        )
    elif file_path.endswith(".cc"):
        print("Generating source file...")
        header_path = file_path.replace(".cc", ".hh")
        create_source_file(repo_dir, file_path, header_path)
    else:
        print("Unsupported file type.")


if __name__ == "__main__":
    main()
