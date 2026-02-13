#!/usr/bin/env bash
# SovereignDroid Development Script
# Primary entry point for build, install, and run workflow

set -e

echo "🚀 SovereignDroid Development Pipeline"
echo "======================================"
echo ""

# Source environment configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/env.sh"

# Function to check for device
check_device() {
    local device_count=$(adb devices | grep -v "List of devices" | grep "device$" | wc -l)
    if [ "$device_count" -eq 0 ]; then
        echo "❌ No Android device connected."
        echo "   Connect a device with USB debugging enabled and try again."
        exit 1
    fi
    echo "📱 Device connected: $(adb devices | grep "device$" | head -n 1 | awk '{print $1}')"
}

# Check device before starting
check_device

# Step 1: Clean (optional, uncomment if needed)
# echo "🧹 Cleaning previous build..."
# ./gradlew clean

# Step 2: Build
echo ""
echo "🔨 Building APK..."
./gradlew assembleDebug

# Check if build succeeded
if [ ! -f "app/build/outputs/apk/debug/app-debug.apk" ]; then
    echo "❌ Build failed: APK not found"
    exit 1
fi

echo "✅ Build successful"

# Step 3: Install
echo ""
echo "📦 Installing APK to device..."
adb install -r app/build/outputs/apk/debug/app-debug.apk

echo "✅ Installation successful"

# Step 4: Launch
echo ""
echo "🎯 Launching SovereignDroid..."
adb shell am start -n com.sovereigndroid.core/com.sovereigndroid.core.MainActivity

echo ""
echo "✅ SovereignDroid launched successfully"
echo ""
echo "📊 To view logs, run:"
echo "   ./log.sh"
echo ""
