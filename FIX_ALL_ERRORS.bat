@echo off
echo ============================================
echo  HWID Randomizer - Complete Fix Script
echo ============================================
echo.

cd /d "%~dp0"

echo Step 1: Checking ImGui installation...
if not exist "external\imgui\imgui.cpp" (
    echo [ERROR] ImGui not found!
    echo Run this first: git submodule update --init --recursive
    echo Or: git clone https://github.com/ocornut/imgui.git external\imgui
    pause
    exit /b 1
)

echo [OK] ImGui core files found
echo.

echo Step 2: Locating ImGui backend files...
if exist "external\imgui\backends\imgui_impl_win32.cpp" (
    echo [OK] Backends found in: external\imgui\backends\
    set IMGUI_BACKEND=external\imgui\backends
    goto :fix_vcxproj
)

if exist "external\imgui\imgui_impl_win32.cpp" (
    echo [OK] Backends found in: external\imgui\
    set IMGUI_BACKEND=external\imgui
    goto :fix_vcxproj
)

echo [ERROR] ImGui backend files not found!
echo Expected files:
echo   - imgui_impl_win32.cpp
echo   - imgui_impl_dx11.cpp
echo.
echo These should be in external\imgui\backends\
pause
exit /b 1

:fix_vcxproj
echo.
echo Step 3: Updating Visual Studio project file...

echo Backing up .vcxproj...
copy /Y HWIDRandomizer.vcxproj HWIDRandomizer.vcxproj.backup >nul

echo Updating ImGui paths in .vcxproj...
powershell -Command "(gc HWIDRandomizer.vcxproj) -replace 'external\\imgui\\imgui_impl', '%IMGUI_BACKEND%\imgui_impl' -replace 'external\\\\imgui\\\\imgui_impl', '%IMGUI_BACKEND%\\imgui_impl' | Set-Content HWIDRandomizer.vcxproj"

echo [OK] Project file updated
echo.

echo Step 4: Updating .vcxproj.filters...
if exist "HWIDRandomizer.vcxproj.filters" (
    copy /Y HWIDRandomizer.vcxproj.filters HWIDRandomizer.vcxproj.filters.backup >nul
    powershell -Command "(gc HWIDRandomizer.vcxproj.filters) -replace 'external\\imgui\\imgui_impl', '%IMGUI_BACKEND%\imgui_impl' -replace 'external\\\\imgui\\\\imgui_impl', '%IMGUI_BACKEND%\\imgui_impl' | Set-Content HWIDRandomizer.vcxproj.filters"
    echo [OK] Filters file updated
) else (
    echo [SKIP] Filters file not found
)

echo.
echo ============================================
echo  All fixes applied successfully!
echo ============================================
echo.
echo NEXT STEPS:
echo 1. Close Visual Studio if open
echo 2. Reopen HWIDRandomizer.sln
echo 3. Select Release ^| x64 at the top
echo 4. Build -^> Rebuild Solution
echo.
echo Backup created: HWIDRandomizer.vcxproj.backup
echo ============================================
pause
