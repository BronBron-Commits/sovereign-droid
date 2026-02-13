# SovereignDroid Makefile
# Provides standard interface for common development operations

.PHONY: help build run clean log install env check

# Default target
help:
	@echo "SovereignDroid Development Commands"
	@echo "===================================="
	@echo ""
	@echo "  make build    - Build debug APK"
	@echo "  make run      - Build, install, and launch app"
	@echo "  make clean    - Clean build artifacts"
	@echo "  make log      - View app logs"
	@echo "  make install  - Install APK to device"
	@echo "  make env      - Show environment configuration"
	@echo "  make check    - Validate development environment"
	@echo ""

# Build debug APK
build:
	@echo "🔨 Building SovereignDroid..."
	@./gradlew assembleDebug

# Full development cycle: build, install, launch
run:
	@./dev.sh

# Clean build artifacts
clean:
	@./clean.sh

# View logs
log:
	@./log.sh

# Install only (requires existing APK)
install:
	@echo "📦 Installing APK..."
	@adb install -r app/build/outputs/apk/debug/app-debug.apk
	@echo "✅ Installation complete"

# Show environment configuration
env:
	@bash -c "source env.sh"

# Validate development environment
check:
	@echo "🔍 Validating development environment..."
	@bash -c "source env.sh && echo '✅ Environment validation complete'"
	@echo ""
	@echo "Checking Gradle wrapper..."
	@./gradlew --version
	@echo ""
	@echo "Checking connected devices..."
	@adb devices
	@echo ""
	@echo "✅ Environment check complete"
