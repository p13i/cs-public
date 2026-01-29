#!/bin/bash
# Quick script to debug auth_test with lldb
# Usage: ./debug_auth.sh [test_name]

TEST="${1:-AuthAPITest.LoginSuccessAfterRegistration}"
BINARY="/workspaces/cs/bazel-bin/cs/apps/trycopilot.ai/api/auth_test"

echo "Building debug binary..."
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk build -c dbg //cs/apps/trycopilot.ai/api:auth_test

echo ""
echo "Starting lldb debugger..."
echo "Suggested breakpoints:"
echo "  break cs::apps::trycopilotai::api::LoginAPI::Process"
echo "  break cs::apps::trycopilot::api::CreateUserAPI::Process"
echo ""
echo "Then type: run --gtest_filter=${TEST}"
echo ""

lldb ${BINARY}
