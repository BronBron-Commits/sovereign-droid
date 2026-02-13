#!/usr/bin/env bash
# SovereignDroid Logging Script
# Filters logcat for SovereignDroid-specific output

set -e

echo "📊 SovereignDroid Logcat (press Ctrl+C to stop)"
echo "=============================================="
echo ""

# Clear logcat buffer first (optional)
# adb logcat -c

# Filter for SovereignDroid package
# Shows only logs from com.sovereigndroid.core
adb logcat --pid=$(adb shell pidof -s com.sovereigndroid.core) 2>/dev/null || adb logcat | grep -i sovereigndroid
