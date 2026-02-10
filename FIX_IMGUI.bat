@echo off
echo ============================================
echo  ImGui Setup Fix Script
echo ============================================
echo.

cd /d "%~dp0"

echo Checking if ImGui exists...
if exist "external\imgui\imgui.cpp" (
    echo [OK] ImGui already exists!
    goto :check_backends
) else (
    echo [MISSING] ImGui not found, downloading...
    goto :download
)

:download
echo.
echo Removing empty imgui folder if exists...
rmdir /s /q external\imgui 2>nul

echo.
echo Initializing ImGui submodule...
git submodule update --init --recursive

if errorlevel 1 (
    echo.
    echo [ERROR] Git submodule failed. Trying manual download...
    goto :manual_download
)

goto :check_backends

:manual_download
echo.
echo Downloading ImGui manually...
mkdir external 2>nul
cd external

echo Cloning ImGui repository...
git clone https://github.com/ocornut/imgui.git imgui

if errorlevel 1 (
    echo [ERROR] Failed to download ImGui!
    echo Please download manually from https://github.com/ocornut/imgui
    pause
    exit /b 1
)

cd ..

:check_backends
echo.
echo Checking ImGui backends...

if exist "external\imgui\backends\imgui_impl_win32.cpp" (
    echo [OK] ImGui backends found in backends\ folder
    goto :fix_project
)

if exist "external\imgui\imgui_impl_win32.cpp" (
    echo [OK] ImGui backends found in root folder
    goto :fix_project
)

echo [ERROR] ImGui backend files not found!
echo Expected location: external\imgui\backends\imgui_impl_*.cpp
echo.
dir external\imgui /w
pause
exit /b 1

:fix_project
echo.
echo [SUCCESS] ImGui is properly installed!
echo.
echo Files found:
dir external\imgui\imgui.cpp 2>nul
dir external\imgui\backends\imgui_impl_*.cpp 2>nul
dir external\imgui\imgui_impl_*.cpp 2>nul

echo.
echo ============================================
echo Now you need to update the .vcxproj file
echo to use the correct ImGui paths.
echo.
echo Run: UPDATE_VCXPROJ.bat
echo ============================================
pause
