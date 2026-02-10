cmake_minimum_required(VERSION 3.15)
project(HWIDRandomizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check - Windows only
if(NOT WIN32)
    message(FATAL_ERROR "This project only supports Windows")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/utils/Logger.cpp
    src/utils/Utils.cpp
    src/backup/BackupManager.cpp
    src/engine/RandomizationEngine.cpp
    src/gui/GuiManager.cpp
    src/modules/SmbiosModule.cpp
    src/modules/TpmModule.cpp
    src/modules/SecureBootModule.cpp
    src/modules/MacAddressModule.cpp
    src/modules/DiskSerialModule.cpp
    src/modules/CpuModule.cpp
    src/modules/GpuModule.cpp
    src/modules/RamModule.cpp
    src/modules/MonitorModule.cpp
    src/modules/EfiModule.cpp
    src/modules/MachineGuidModule.cpp
    src/modules/UsbModule.cpp
)

# ImGui sources (when added)
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_impl_win32.cpp
    external/imgui/imgui_impl_dx11.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/imgui
)

# Create executable
add_executable(HWIDRandomizer WIN32 ${SOURCES} ${IMGUI_SOURCES})

# Windows-specific settings
if(MSVC)
    # Set subsystem to Windows (GUI application)
    set_target_properties(HWIDRandomizer PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /MANIFESTINPUT:${CMAKE_SOURCE_DIR}/app.manifest"
    )

    # Enable parallel compilation
    target_compile_options(HWIDRandomizer PRIVATE /MP)

    # Static runtime linking for standalone executable
    set_property(TARGET HWIDRandomizer PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Preprocessor definitions
target_compile_definitions(HWIDRandomizer PRIVATE
    WIN32
    _WINDOWS
    UNICODE
    _UNICODE
    IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
)

# Link libraries
target_link_libraries(HWIDRandomizer
    d3d11.lib
    dxgi.lib
    setupapi.lib
    wbemuuid.lib
    iphlpapi.lib
    bcrypt.lib
    taskschd.lib
    ntdll.lib
    shlwapi.lib
    ole32.lib
    oleaut32.lib
    advapi32.lib
    shell32.lib
)

# Installation
install(TARGETS HWIDRandomizer
    RUNTIME DESTINATION bin
)

# Print build info
message(STATUS "===========================================")
message(STATUS "  HWID Randomizer Build Configuration")
message(STATUS "===========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")
