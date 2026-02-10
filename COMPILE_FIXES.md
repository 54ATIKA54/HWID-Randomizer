# Compilation Fixes

## Step 1: Add ImGui Library (CRITICAL - Do This First!)

**Close Visual Studio**, then run:

```bash
cd D:\HWID-Randomizer
git submodule add https://github.com/ocornut/imgui.git external/imgui
git submodule update --init --recursive
```

**Verify ImGui is present:**
```bash
dir external\imgui\imgui.cpp
dir external\imgui\imgui_impl_win32.cpp
dir external\imgui\imgui_impl_dx11.cpp
```

All three files should exist.

## Step 2: Switch to Release x64

In Visual Studio at the top toolbar:
- Change **Debug** → **Release**
- Keep **x64**

## Step 3: Reload Solution

1. In Visual Studio: File → Close Solution
2. Double-click `HWIDRandomizer.sln` to reopen
3. If prompted about missing files, click OK

## Step 4: Rebuild

- Build → Rebuild Solution (Ctrl+Shift+B)
- Wait 2-3 minutes

## Common Errors & Fixes

### Error: "external\imgui\*.cpp not found"
**Cause:** ImGui submodule not added
**Fix:** Do Step 1 above, then restart Visual Studio

### Error: "MotherboardProfile unknown identifier"
**Fixed:** I just created `include/Common.h` with the enum
**Action:** Reload solution in VS

### Error: "IP_ADAPTER_ADDRESSES undeclared"
**Fixed:** I added `#include <winsock2.h>` before `<Windows.h>`
**Action:** Should compile now

### Error: "sprintf_s not enough arguments"
**Fixed:** I added `sizeof(macStr)` parameter
**Action:** Should compile now

### Error: German error messages
**Info:** Your VS is in German
- "Unbekannter Überschreibungsspezifizierer" = Unknown override specifier
- "nichtdeklarierter Bezeichner" = undeclared identifier
These are normal C++ errors, just in German.

## If Still Getting Errors After Steps 1-4

### Missing Windows SDK
**Error:** "d3d11.lib not found" or similar
**Fix:**
1. Open Visual Studio Installer
2. Modify → Individual Components
3. Check "Windows 10 SDK (10.0.19041.0)" or higher
4. Install

### Wrong Platform Toolset
**Error:** Toolset v143 not found
**Fix:**
1. Right-click project in Solution Explorer
2. Properties → General
3. Platform Toolset → Select available version (v142 or v143)
4. Apply

### Retarget Solution
**Prompt:** "Retarget Projects"
**Fix:** Click OK and select your installed SDK version

## Expected Build Output

**Success:**
```
Build succeeded.

1 succeeded, 0 failed, 0 up-to-date, 0 skipped
```

**Output location:**
```
D:\HWID-Randomizer\build\x64\Release\HWIDRandomizer.exe
```

## After Successful Build

```bash
# Check file exists
dir build\x64\Release\HWIDRandomizer.exe

# Run (right-click → Run as administrator)
```

## Files I Just Fixed

1. ✅ Created `include/Common.h` - Contains MotherboardProfile enum
2. ✅ Updated `include/modules/SmbiosModule.h` - Uses Common.h
3. ✅ Updated `include/engine/RandomizationEngine.h` - Uses Common.h
4. ✅ Fixed `src/modules/MacAddressModule.cpp` - Fixed includes and sprintf_s

## What You Need To Do

1. **Add ImGui submodule** (git command above)
2. **Reload Visual Studio**
3. **Switch to Release x64**
4. **Build**

That's it!
