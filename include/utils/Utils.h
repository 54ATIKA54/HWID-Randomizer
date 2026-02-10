#pragma once

#include <string>
#include <Windows.h>
#include <vector>

namespace HWIDRandomizer {
namespace Utils {

    // Random generation functions
    std::string GenerateRandomMAC(bool useValidOUI = true);
    std::string GenerateRandomSerial(int length, const std::string& prefix = "");
    std::string GenerateRandomGUID();
    std::string GenerateRandomHex(int bytes);

    // OS detection
    bool IsWindows10();
    std::string GetWindowsVersion();

    // Registry operations
    std::string ReadRegistryString(HKEY root, const std::string& path, const std::string& valueName);
    DWORD ReadRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName);
    bool WriteRegistryString(HKEY root, const std::string& path, const std::string& valueName, const std::string& value);
    bool WriteRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName, DWORD value);
    bool RegistryKeyExists(HKEY root, const std::string& path);

    // Command execution
    std::string ExecuteCommand(const std::string& command, int* exitCode = nullptr);
    bool ExecuteCommandSilent(const std::string& command);

    // WMI operations
    struct WMIProperty {
        std::string name;
        std::string value;
    };
    std::vector<WMIProperty> QueryWMI(const std::string& wmiClass, const std::vector<std::string>& properties);

    // Network adapter operations
    bool RestartNetworkAdapter(const std::string& adapterName);

    // File operations
    bool CreateDirectoryIfNotExists(const std::string& path);
    bool FileExists(const std::string& path);
    std::string GetTimestampString();

    // Admin checks
    bool IsRunningAsAdministrator();
    bool EnablePrivilege(const std::string& privilegeName);

    // String utilities
    std::wstring StringToWString(const std::string& str);
    std::string WStringToString(const std::wstring& wstr);
    std::string ToUpper(const std::string& str);
    std::string ToLower(const std::string& str);

} // namespace Utils
} // namespace HWIDRandomizer
