@echo off
echo ============================================
echo  HWID Randomizer - ImGui Setup
echo ============================================
echo.

cd /d "%~dp0"

echo Checking if git is available...
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Git not found!
    echo.
    echo Please install Git from: https://git-scm.com/
    echo Or download ImGui manually from: https://github.com/ocornut/imgui
    echo.
    pause
    exit /b 1
)

echo [OK] Git found
echo.

echo Downloading ImGui library...
if exist "external\imgui\.git" (
    echo ImGui already exists, updating...
    cd external\imgui
    git pull
    cd ..\..
) else (
    echo Cloning ImGui repository...
    rmdir /s /q external\imgui 2>nul
    git clone --depth 1 https://github.com/ocornut/imgui.git external\imgui
)

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to download ImGui
    echo.
    echo Please download manually:
    echo 1. Go to: https://github.com/ocornut/imgui
    echo 2. Click "Code" -> "Download ZIP"
    echo 3. Extract to external\imgui\
    echo.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] ImGui downloaded successfully!
echo.

echo Verifying required files...
set MISSING=0

if not exist "external\imgui\imgui.cpp" (
    echo [MISSING] imgui.cpp
    set MISSING=1
)
if not exist "external\imgui\imgui_draw.cpp" (
    echo [MISSING] imgui_draw.cpp
    set MISSING=1
)
if not exist "external\imgui\imgui_widgets.cpp" (
    echo [MISSING] imgui_widgets.cpp
    set MISSING=1
)
if not exist "external\imgui\imgui_tables.cpp" (
    echo [MISSING] imgui_tables.cpp
    set MISSING=1
)
if not exist "external\imgui\backends\imgui_impl_win32.cpp" (
    echo [MISSING] backends\imgui_impl_win32.cpp
    set MISSING=1
)
if not exist "external\imgui\backends\imgui_impl_dx11.cpp" (
    echo [MISSING] backends\imgui_impl_dx11.cpp
    set MISSING=1
)

if %MISSING% EQU 1 (
    echo.
    echo [ERROR] Some ImGui files are missing!
    echo Please check the external\imgui\ directory
    pause
    exit /b 1
)

echo [OK] All required ImGui files present
echo.

echo Checking if backends path is correct in project...
findstr /C:"external\imgui\backends\imgui_impl_win32.cpp" HWIDRandomizer.vcxproj >nul
if %ERRORLEVEL% EQU 0 (
    echo [OK] Project file has correct paths
) else (
    echo [WARNING] Project file may need path updates
    echo Run FIX_ALL_ERRORS.bat if you get compile errors
)

echo.
echo ============================================
echo  Setup Complete!
echo ============================================
echo.
echo Next steps:
echo 1. Open HWIDRandomizer.sln in Visual Studio
echo 2. Select "Release" and "x64" configuration
echo 3. Build -^> Rebuild Solution
echo.
echo If you get path errors, run: FIX_ALL_ERRORS.bat
echo ============================================
pause
