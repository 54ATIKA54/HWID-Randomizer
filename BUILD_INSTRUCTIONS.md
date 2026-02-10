# HWID Randomizer - Complete Build Instructions

## Quick Start

### Prerequisites
- Windows 10 x64
- Visual Studio 2019/2022 (Community or higher)
- Git
- 2GB free disk space

### Build Steps

```bash
# 1. Clone repository
git clone <your-repo-url>
cd HWID-Randomizer

# 2. Initialize ImGui submodule
git submodule add https://github.com/ocornut/imgui.git external/imgui
git submodule update --init --recursive

# 3. Build with CMake
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 4. Run (as administrator)
cd Release
# Right-click HWIDRandomizer.exe -> Run as administrator
```

---

## Detailed Setup

### Step 1: Install Prerequisites

#### Visual Studio 2022 (Recommended)

1. Download from: https://visualstudio.microsoft.com/
2. Run installer
3. Select **"Desktop development with C++"** workload
4. Ensure these components are checked:
   - MSVC v143 (or latest)
   - Windows 10 SDK (10.0.19041.0 or later)
   - C++ CMake tools for Windows
5. Install (8-10 GB required)

#### Git

1. Download from: https://git-scm.com/
2. Install with default options

### Step 2: Clone and Setup Project

```bash
# Clone repository
git clone <your-repo-url>
cd HWID-Randomizer

# Add ImGui as submodule
git submodule add https://github.com/ocornut/imgui.git external/imgui
git submodule update --init --recursive

# Verify structure
dir /s /b *.cpp | findstr /i "imgui"
# Should show imgui source files
```

### Step 3: Build with CMake

#### Option A: Command Line (Recommended)

```cmd
# Open "x64 Native Tools Command Prompt for VS 2022"
cd HWID-Randomizer

# Generate build files
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

# Build Release configuration
cmake --build . --config Release

# Output location
dir Release\HWIDRandomizer.exe
```

#### Option B: Visual Studio CMake Integration

1. Open Visual Studio 2022
2. File → Open → CMake...
3. Select `CMakeLists.txt` in project root
4. Wait for CMake cache generation
5. Build → Build All (Ctrl+Shift+B)
6. Output: `out/build/x64-Release/HWIDRandomizer.exe`

### Step 4: Manual Visual Studio Solution (Alternative)

If you prefer traditional .sln workflow:

1. **Create blank solution:**
   - Visual Studio → Create New Project
   - Empty Project (C++)
   - Name: HWIDRandomizer
   - Location: Project root

2. **Add source files:**
   - Solution Explorer → Right-click project → Add → Existing Item
   - Add all .cpp files from `src/`
   - Add all .cpp files from `external/imgui/` (imgui.cpp, imgui_draw.cpp, imgui_widgets.cpp, imgui_tables.cpp, imgui_impl_win32.cpp, imgui_impl_dx11.cpp)

3. **Configure include directories:**
   - Project Properties → C/C++ → General → Additional Include Directories
   - Add: `$(ProjectDir)include`
   - Add: `$(ProjectDir)external\imgui`

4. **Configure linker:**
   - Project Properties → Linker → Input → Additional Dependencies
   - Add all libraries from CMakeLists.txt:
     ```
     d3d11.lib;dxgi.lib;setupapi.lib;wbemuuid.lib;iphlpapi.lib;bcrypt.lib;taskschd.lib;ntdll.lib;shlwapi.lib;ole32.lib;oleaut32.lib;advapi32.lib;shell32.lib
     ```

5. **Embed manifest:**
   - Project Properties → Manifest Tool → Input and Output
   - Additional Manifest Files: `$(ProjectDir)app.manifest`

6. **Build:**
   - Configuration: Release
   - Platform: x64
   - Build → Build Solution

### Step 5: Verify Build

```cmd
# Check executable exists
dir build\Release\HWIDRandomizer.exe

# Check file size (should be 2-3 MB)
# Check dependencies (should be minimal)
dumpbin /dependents build\Release\HWIDRandomizer.exe
```

Expected dependencies:
- KERNEL32.dll
- USER32.dll
- d3d11.dll
- dxgi.dll
- ADVAPI32.dll
- SHELL32.dll
- ole32.dll
- (and other system DLLs)

---

## Troubleshooting

### Issue: "ImGui files not found"

```bash
# Verify submodule
git submodule status
# Should show: <hash> external/imgui (vX.XX)

# If missing, initialize:
git submodule update --init --recursive
```

### Issue: "Cannot open include file: 'd3d11.h'"

**Solution:** Install Windows 10 SDK via Visual Studio Installer
1. Open Visual Studio Installer
2. Modify → Individual Components
3. Check "Windows 10 SDK (10.0.19041.0)" or later
4. Install

### Issue: "LNK2019: unresolved external symbol"

**Cause:** Missing library in linker settings

**Solution:** Verify all libraries listed in CMakeLists.txt are added to linker input

### Issue: "MSVCR140.dll missing" when running

**Cause:** Dynamic runtime linking

**Solution:** Build with static runtime
- CMake handles this automatically (`/MT` flag)
- Or install Visual C++ Redistributable: https://aka.ms/vs/17/release/vc_redist.x64.exe

### Issue: Executable crashes on startup

**Possible causes:**
1. Not running as administrator → Right-click → Run as administrator
2. Windows 11 detected → Only Windows 10 supported
3. Missing DLLs → Check with Dependency Walker

### Issue: CMake generation fails

```bash
# Clear cache and regenerate
rd /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

---

## Development Build

For debugging and development:

```bash
# Debug build
cmake --build . --config Debug

# Output includes PDB symbols
dir Debug\HWIDRandomizer.exe
dir Debug\HWIDRandomizer.pdb
```

Debug build enables:
- Full debugging symbols
- Assert checks
- Runtime checks
- Console window for debug output

---

## Advanced Configuration

### Custom Build Flags

```bash
# With specific Windows SDK
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_SYSTEM_VERSION=10.0.22000.0

# With additional definitions
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_FLAGS="/DTEST_MODE"
```

### Static Linking Everything

Modify CMakeLists.txt:
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

This creates fully standalone executable with no MSVC runtime dependencies.

---

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build HWID Randomizer

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
        with:
          submodules: recursive

      - name: Setup MSBuild
        uses: microsoft/setup-msbuild@v1

      - name: CMake Configure
        run: cmake -B build -G "Visual Studio 17 2022" -A x64

      - name: CMake Build
        run: cmake --build build --config Release

      - name: Upload Artifact
        uses: actions/upload-artifact@v3
        with:
          name: HWIDRandomizer-x64
          path: build/Release/HWIDRandomizer.exe
```

---

## Distribution

### Creating Release Package

1. Build Release configuration
2. Create distribution folder:
   ```cmd
   mkdir HWIDRandomizer-v1.0.0
   copy build\Release\HWIDRandomizer.exe HWIDRandomizer-v1.0.0\
   copy README.md HWIDRandomizer-v1.0.0\
   copy app.manifest HWIDRandomizer-v1.0.0\
   xcopy /E /I docs HWIDRandomizer-v1.0.0\docs
   ```

3. Create ZIP:
   ```cmd
   powershell Compress-Archive -Path HWIDRandomizer-v1.0.0 -DestinationPath HWIDRandomizer-v1.0.0-x64.zip
   ```

4. Test on clean Windows 10 VM

### Portable Executable

The Release build is already portable:
- No installer needed
- No external dependencies (static runtime)
- Only requires Windows 10 + admin privileges
- Writes data to C:\HWIDBackup only

---

## Performance Optimization

### Compiler Optimizations (Already Enabled in Release)

- `/O2` - Maximum optimization
- `/Ob2` - Inline function expansion
- `/Oi` - Enable intrinsic functions
- `/Ot` - Favor fast code
- `/Oy` - Omit frame pointers
- `/GL` - Whole program optimization
- `/LTCG` - Link-time code generation

### Build Time Optimization

```cmd
# Parallel compilation (8 threads)
cmake --build . --config Release -- /m:8

# Or in Visual Studio project properties:
# C/C++ → General → Multi-processor Compilation → Yes (/MP)
```

---

## Next Steps

After successful build:
1. Read `README.md` for usage instructions
2. Review `docs/USAGE.md` for detailed operation guide
3. **Test in VM first** before using on physical hardware
4. Enable logging and monitor `C:\HWIDBackup\operation_log_*.txt`

---

## Support

**Build issues:** Check this document and CMakeLists.txt
**Runtime issues:** Check logs in C:\HWIDBackup\
**Feature requests:** See API_REFERENCE.md for extension points

**Last Updated:** 2024
**Tool Version:** 1.0.0
**Build System:** CMake 3.15+ / Visual Studio 2019/2022
