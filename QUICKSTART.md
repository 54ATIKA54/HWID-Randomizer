# HWID Randomizer - Quick Start Guide

## ⚠️ READ THIS FIRST

**This tool is for legitimate research in controlled environments only.**
- Use only in VMs or systems you fully own
- Not for bypassing bans, anti-cheat, or ToS violations
- Windows 10 only (not Windows 11)
- Requires administrator privileges

---

## Quick Build & Run (Visual Studio)

### Step 1: Setup ImGui

Before opening Visual Studio, you **must** add the ImGui library:

```bash
cd D:\HWID-Randomizer

# Add ImGui as submodule
git submodule add https://github.com/ocornut/imgui.git external/imgui

# Initialize it
git submodule update --init --recursive
```

**Verify ImGui is present:**
```bash
dir external\imgui\imgui.cpp
```
You should see the file listed.

### Step 2: Open in Visual Studio

1. **Double-click:** `HWIDRandomizer.sln`
2. Visual Studio will load the project
3. If prompted to retarget SDK, click **OK** (or select Windows 10 SDK version available on your system)

### Step 3: Select Configuration

At the top toolbar:
- Configuration: **Release**
- Platform: **x64**

### Step 4: Build

- **Menu:** Build → Build Solution
- **Or press:** Ctrl+Shift+B
- Wait 2-3 minutes for compilation

### Step 5: Locate Executable

After successful build:
```
D:\HWID-Randomizer\build\x64\Release\HWIDRandomizer.exe
```

### Step 6: Run (as Administrator)

- **Right-click** the .exe → "Run as administrator"
- Or double-click (UAC prompt will appear)

---

## Build Troubleshooting

### Error: "Cannot open source file 'external/imgui/imgui.h'"

**Cause:** ImGui submodule not initialized

**Fix:**
```bash
git submodule add https://github.com/ocornut/imgui.git external/imgui
git submodule update --init --recursive
```

Then reload the solution in Visual Studio.

### Error: "Windows SDK not found" or "Retarget solution"

**Cause:** Project configured for SDK not installed on your system

**Fix:**
1. Right-click project in Solution Explorer
2. Properties → General → Windows SDK Version
3. Select an installed SDK (e.g., 10.0.19041.0 or 10.0.22000.0)
4. Apply and rebuild

### Error: "d3d11.lib not found"

**Cause:** Windows SDK not properly installed

**Fix:**
1. Open Visual Studio Installer
2. Modify your installation
3. Individual Components → Check "Windows 10 SDK (10.0.19041.0)" or later
4. Install and restart Visual Studio

### Error: "LINK : fatal error LNK1104: cannot open file 'kernel32.lib'"

**Cause:** Platform toolset or SDK path issue

**Fix:**
1. Project Properties → General → Platform Toolset
2. Select an installed toolset (v143 for VS2022, v142 for VS2019)
3. Apply and rebuild

### Error: Build succeeds but exe won't run

**Cause:** Missing administrator privileges or wrong OS

**Fix:**
1. **Always** right-click .exe → "Run as administrator"
2. Verify you're on Windows 10 (not Windows 11): `winver`
3. Check log: `C:\HWIDBackup\operation_log_*.txt`

---

## Alternative: CMake Build

If you prefer CMake (Visual Studio not required for editing):

```bash
cd D:\HWID-Randomizer

# Add ImGui first
git submodule add https://github.com/ocornut/imgui.git external/imgui
git submodule update --init --recursive

# Generate build files
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release

# Output
dir Release\HWIDRandomizer.exe
```

---

## After Building

### First Run

1. Right-click `HWIDRandomizer.exe` → **Run as administrator**
2. Dark-themed GUI window appears
3. Default settings:
   - Mode: **Persistent (Survives Reboot)**
   - Profile: **MSI MAG X670E TOMAHAWK WIFI**
   - Safety: **System Restore Point ✓**, **Backup ✓**

### Test in VM (Strongly Recommended)

Before using on physical hardware:
1. Set up Windows 10 VM (VMware, VirtualBox, Hyper-V)
2. Take VM snapshot
3. Copy executable to VM
4. Test randomization
5. Verify changes with `msinfo32`, `ipconfig /all`, etc.
6. Test revert functionality
7. Restore VM snapshot if issues occur

### Basic Operation

1. Click **"▶ APPLY FULL RANDOMIZATION"**
2. Watch progress bar and console log
3. Wait for completion message
4. Reboot if prompted
5. Verify changes:
   ```cmd
   msinfo32
   wmic baseboard get serialnumber
   ipconfig /all
   ```

### Revert Changes

1. Click **"🔄 FULL REVERT"**
2. Select backup timestamp
3. Confirm restore
4. Reboot

### View Logs

All operations logged to:
```
C:\HWIDBackup\operation_log_YYYYMMDD_HHMMSS.txt
```

---

## File Structure After Setup

```
D:\HWID-Randomizer\
├── HWIDRandomizer.sln          ← Open this in Visual Studio
├── HWIDRandomizer.vcxproj
├── HWIDRandomizer.vcxproj.filters
├── external\
│   └── imgui\                  ← Must be present (from git submodule)
│       ├── imgui.cpp
│       ├── imgui.h
│       └── ... (many files)
├── src\                        ← Source code
├── include\                    ← Headers
├── build\                      ← Build output (created after build)
│   └── x64\
│       └── Release\
│           └── HWIDRandomizer.exe  ← Your executable
└── docs\                       ← Documentation
```

---

## Next Steps

- **Documentation:** Read `README.md` for detailed features
- **Usage Guide:** See `docs/USAGE.md` for step-by-step operations
- **Kernel Driver:** See `docs/KERNEL_DRIVER_SETUP.md` for advanced features (optional)
- **API Reference:** See `docs/API_REFERENCE.md` for development

---

## Key Reminders

✅ **Always run as administrator** - Required for registry/hardware access
✅ **Windows 10 only** - Tool explicitly checks and rejects Windows 11
✅ **Use in VM first** - Test before using on physical hardware
✅ **Enable backups** - Safety options should stay checked
✅ **Read logs** - All operations detailed in `C:\HWIDBackup\`

❌ **Not for anti-cheat bypass** - Unethical and likely ineffective
❌ **Not for ban evasion** - Violates ToS and may worsen situation
❌ **Not for production systems** - Test environments only

---

## Support

**Build issues:** Check BUILD_INSTRUCTIONS.md
**Runtime issues:** Check logs in C:\HWIDBackup\
**OS compatibility:** Windows 10 only, x64 architecture

---

**Ready to build? Start with Step 1: Setup ImGui**
