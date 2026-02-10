@echo off
echo ============================================
echo  Visual Studio Project Update Script
echo ============================================
echo.

cd /d "%~dp0"

echo Backing up original .vcxproj...
copy HWIDRandomizer.vcxproj HWIDRandomizer.vcxproj.backup >nul 2>&1

echo.
echo Checking ImGui location...

if exist "external\imgui\backends\imgui_impl_win32.cpp" (
    echo [FOUND] ImGui backends in: external\imgui\backends\
    set IMGUI_PATH=external\imgui\backends
    goto :update
)

if exist "external\imgui\imgui_impl_win32.cpp" (
    echo [FOUND] ImGui backends in: external\imgui\
    set IMGUI_PATH=external\imgui
    goto :update
)

echo [ERROR] ImGui backend files not found!
echo Please run FIX_IMGUI.bat first.
pause
exit /b 1

:update
echo.
echo Updating .vcxproj file to use: %IMGUI_PATH%
echo.

powershell -Command "(gc HWIDRandomizer.vcxproj) -replace 'external\\imgui\\imgui_impl_win32.cpp', '%IMGUI_PATH%\imgui_impl_win32.cpp' | Set-Content HWIDRandomizer.vcxproj"
powershell -Command "(gc HWIDRandomizer.vcxproj) -replace 'external\\imgui\\imgui_impl_dx11.cpp', '%IMGUI_PATH%\imgui_impl_dx11.cpp' | Set-Content HWIDRandomizer.vcxproj"

echo [SUCCESS] Project file updated!
echo.
echo ============================================
echo Now reload the solution in Visual Studio:
echo 1. Close the solution
echo 2. Reopen HWIDRandomizer.sln
echo 3. Build -^> Rebuild Solution
echo ============================================
pause
