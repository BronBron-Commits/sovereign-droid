#!/usr/bin/env bash
# SovereignDroid Clean Script
# Removes all build artifacts

set -e

echo "🧹 Cleaning SovereignDroid build artifacts..."

# Source environment
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh"

# Run Gradle clean
./gradlew clean

echo "✅ Clean complete"
