# Phase 0.1 — Baseline Verification

**Status**: ✅ VERIFIED  
**Objective**: Prove the baseline is reproducible and deterministic before any further development.
**Verified Date**: 2026-02-12

---

## Critical Principle

**You do not build on unverified infrastructure.**

This phase validates that:
- The environment setup is complete and documented
- The build is reproducible
- The workflow is deterministic
- No manual intervention is required

---

## Verification Checklist

### ✅ Step 1: Clean Machine Test

Simulate a fresh developer onboarding:

1. **Clone repository** (fresh, no cached state)
   ```bash
   git clone <repo-url> SovereignDroid-Fresh
   cd SovereignDroid-Fresh
   ```

2. **Follow only documented setup**
   - Read `DEV_ENV.md`
   - Set `JAVA_HOME` and `ANDROID_HOME`
   - Install required SDK components
   - Connect physical device

3. **Execute primary workflow**
   ```bash
   ./dev.sh
   ```

4. **Expected outcome**:
   - ✅ Builds without errors
   - ✅ Installs to device
   - ✅ Launches successfully
   - ✅ No manual fixes required

5. **Validate logging**
   ```bash
   ./log.sh
   ```
   - ✅ Logs appear
   - ✅ App is running

**If ANY manual step was required** → Update `DEV_ENV.md` immediately.

---

### ✅ Step 2: Deterministic Build Check

Validate version stability:

1. **Clean and rebuild**
   ```bash
   ./clean.sh
   ./dev.sh
   ```

2. **Check for drift**
   - ❌ No version warnings
   - ❌ No dependency auto-upgrades
   - ❌ No Gradle plugin version mismatches
   - ❌ No "newer version available" messages

3. **Rebuild again**
   ```bash
   ./clean.sh
   ./dev.sh
   ```

4. **Confirm identical behavior**
   - Same build output
   - Same APK size (within bytes)
   - Same install behavior

**If ANY version drift occurs** → Tighten `VERSION_LOCK.md`.

---

### ✅ Step 3: Device Matrix Test

**Minimum validation**:
- One physical device (Android 7.0+)

**Optional but recommended**:
- One emulator (if available, to confirm compatibility)

**Test on each target**:
```bash
./dev.sh
```

**Confirm**:
- ✅ Same build works on all targets
- ✅ No ABI mismatch errors
- ✅ No native library loading errors (none expected yet)
- ✅ Consistent launch behavior

---

### ✅ Step 4: Workflow Validation

Test all standard commands work:

```bash
# Test Make interface
make check
make build
make clean
make run

# Test script interface
./clean.sh
./dev.sh
./log.sh  # (Ctrl+C to stop)
```

**All commands must succeed without modification.**

---

### ✅ Step 5: Documentation Accuracy

Review each document for accuracy:

- [ ] `DEV_ENV.md` - All steps work as written
- [ ] `VERSION_LOCK.md` - Versions match actual build
- [ ] `ARCHITECTURE.md` - Decisions accurately reflect current state
- [ ] `env.sh` - All paths and checks are correct
- [ ] `dev.sh` - Workflow executes successfully
- [ ] `Makefile` - All targets work

**Update any inaccuracies immediately.**

---

## Verification Failure Protocol

If **any** verification step fails:

1. **Stop immediately** - do not proceed to Phase 1
2. Document the failure clearly
3. Fix the issue
4. Update relevant documentation
5. Re-run full verification from Step 1

---

## Verification Sign-Off

```
Date: 2026-02-12
Engineer: Verified
Device Tested: Physical Android Device
JDK Version: 17
Gradle Version: 8.13

[✅] Step 1: Clean machine test - PASS
[✅] Step 2: Deterministic build - PASS
    - SHA256 of APK matches across clean builds
    - No version drift detected
[✅] Step 3: Device matrix - PASS
    - Install verified on physical device
    - ABI stable
[✅] Step 4: Workflow validation - PASS
    - All scripts execute successfully
    - Logs show expected package activity
[✅] Step 5: Documentation accuracy - PASS
    - No manual steps required beyond documented

Baseline Status: VERIFIED ✅
```

---

## After Verification

Only after baseline is proven:

**Proceed to Phase 1**

Do NOT:
- Add features
- Refactor code
- Add dependencies
- Implement UI changes
- Add complexity

**Next**: See `PHASE_1.md` for native core bootstrap specification.

---

**Created**: 2026-02-12  
**Purpose**: Lock the foundation before building on it
