#!/usr/bin/env bash
# SovereignDroid Environment Configuration
# Single source of truth for all environment variables
# Source this file before any build operation

set -e

echo "🔧 Configuring SovereignDroid environment..."

# Java Development Kit
# Must be JDK 17 for AGP 8.13.2
if [ -z "$JAVA_HOME" ]; then
    echo "⚠️  JAVA_HOME not set. Please set it to JDK 17 location."
    echo "   Example: export JAVA_HOME=/c/Program\ Files/Java/jdk-17"
    exit 1
fi

# Verify JDK version
JAVA_VERSION=$("$JAVA_HOME/bin/java" -version 2>&1 | head -n 1 | cut -d'"' -f2 | cut -d'.' -f1)
if [ "$JAVA_VERSION" -lt 17 ]; then
    echo "❌ JDK version must be 17 or higher. Found: $JAVA_VERSION"
    exit 1
fi

# Android SDK
if [ -z "$ANDROID_HOME" ]; then
    echo "⚠️  ANDROID_HOME not set. Please set it to your Android SDK location."
    echo "   Example: export ANDROID_HOME=/c/Users/$USER/AppData/Local/Android/Sdk"
    exit 1
fi

# Standard Android environment variables
export ANDROID_SDK_ROOT="$ANDROID_HOME"

# Extend PATH
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"

# Build configuration
export GRADLE_OPTS="-Xmx2g -XX:MaxMetaspaceSize=512m"

# Validation
if [ ! -d "$JAVA_HOME" ]; then
    echo "❌ JAVA_HOME directory does not exist: $JAVA_HOME"
    exit 1
fi

if [ ! -d "$ANDROID_HOME" ]; then
    echo "❌ ANDROID_HOME directory does not exist: $ANDROID_HOME"
    exit 1
fi

# Check for required SDK components
REQUIRED_PLATFORM="android-36"
if [ ! -d "$ANDROID_HOME/platforms/$REQUIRED_PLATFORM" ]; then
    echo "⚠️  Required Android SDK platform not found: $REQUIRED_PLATFORM"
    echo "   Install with: sdkmanager \"platforms;$REQUIRED_PLATFORM\""
fi

# Check for connected device
ADB_DEVICES=$(adb devices | grep -v "List of devices" | grep "device$" | wc -l)
if [ "$ADB_DEVICES" -eq 0 ]; then
    echo "⚠️  No Android device connected. Connect device with USB debugging enabled."
fi

echo "✅ Environment configured:"
echo "   JAVA_HOME:        $JAVA_HOME"
echo "   ANDROID_HOME:     $ANDROID_HOME"
echo "   JDK Version:      $JAVA_VERSION"
echo "   Connected devices: $ADB_DEVICES"
echo ""
