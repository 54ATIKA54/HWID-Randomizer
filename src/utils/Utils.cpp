#include "../../include/utils/Utils.h"
#include "../../include/utils/Logger.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <comdef.h>
#include <Wbemidl.h>
#include <bcrypt.h>
#include <shlobj.h>
#include <shellapi.h>
#include <TlHelp32.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

namespace HWIDRandomizer {
namespace Utils {

    // Random generator
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::string GenerateRandomMAC(bool useValidOUI) {
        unsigned char mac[6];

        // Generate random bytes
        BCRYPT_ALG_HANDLE hAlg = NULL;
        if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RNG_ALGORITHM, NULL, 0) == 0) {
            BCryptGenRandom(hAlg, mac, 6, 0);
            BCryptCloseAlgorithmProvider(hAlg, 0);
        } else {
            // Fallback to std::random
            for (int i = 0; i < 6; i++) {
                mac[i] = static_cast<unsigned char>(gen() % 256);
            }
        }

        // Set locally administered bit and unicast bit
        if (useValidOUI) {
            mac[0] = (mac[0] & 0xFE) | 0x02; // Set bit 1 (locally administered), clear bit 0 (unicast)
        }

        std::stringstream ss;
        for (int i = 0; i < 6; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)mac[i];
            if (i < 5) ss << ":";
        }
        return ToUpper(ss.str());
    }

    std::string GenerateRandomSerial(int length, const std::string& prefix) {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

        std::string serial = prefix;
        for (int i = 0; i < length; i++) {
            serial += charset[dis(gen)];
        }
        return serial;
    }

    std::string GenerateRandomGUID() {
        GUID guid;
        CoCreateGuid(&guid);

        wchar_t guidString[40];
        StringFromGUID2(guid, guidString, 40);

        return WStringToString(std::wstring(guidString));
    }

    std::string GenerateRandomHex(int bytes) {
        std::stringstream ss;
        std::uniform_int_distribution<> dis(0, 255);

        for (int i = 0; i < bytes; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
        }
        return ToUpper(ss.str());
    }

    bool IsWindows10() {
        OSVERSIONINFOEXW osvi = { sizeof(osvi) };
        DWORDLONG const dwlConditionMask = VerSetConditionMask(
            VerSetConditionMask(
                VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL),
                VER_MINORVERSION, VER_EQUAL),
            VER_BUILDNUMBER, VER_GREATER_EQUAL);

        osvi.dwMajorVersion = 10;
        osvi.dwMinorVersion = 0;
        osvi.dwBuildNumber = 10240; // First Windows 10 build

        if (VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask)) {
            // Check it's not Windows 11 (build 22000+)
            DWORD buildNumber = 0;
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD dataSize = sizeof(DWORD);
                DWORD type = REG_DWORD;
                wchar_t buildStr[256];
                DWORD buildStrSize = sizeof(buildStr);

                if (RegQueryValueExW(hKey, L"CurrentBuildNumber", NULL, &type, (LPBYTE)buildStr, &buildStrSize) == ERROR_SUCCESS) {
                    buildNumber = _wtoi(buildStr);
                }
                RegCloseKey(hKey);
            }

            if (buildNumber >= 22000) {
                return false; // Windows 11
            }
            return true;
        }
        return false;
    }

    std::string GetWindowsVersion() {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t productName[256] = {0};
            DWORD dataSize = sizeof(productName);

            if (RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)productName, &dataSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return WStringToString(std::wstring(productName));
            }
            RegCloseKey(hKey);
        }
        return "Unknown";
    }

    std::string ReadRegistryString(HKEY root, const std::string& path, const std::string& valueName) {
        HKEY hKey;
        std::wstring wpath = StringToWString(path);
        std::wstring wvalueName = StringToWString(valueName);

        if (RegOpenKeyExW(root, wpath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t buffer[1024] = {0};
            DWORD bufferSize = sizeof(buffer);
            DWORD type;

            if (RegQueryValueExW(hKey, wvalueName.c_str(), NULL, &type, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                if (type == REG_SZ || type == REG_EXPAND_SZ) {
                    return WStringToString(std::wstring(buffer));
                }
            }
            RegCloseKey(hKey);
        }
        return "";
    }

    DWORD ReadRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName) {
        HKEY hKey;
        std::wstring wpath = StringToWString(path);
        std::wstring wvalueName = StringToWString(valueName);

        if (RegOpenKeyExW(root, wpath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD value = 0;
            DWORD bufferSize = sizeof(DWORD);
            DWORD type;

            if (RegQueryValueExW(hKey, wvalueName.c_str(), NULL, &type, (LPBYTE)&value, &bufferSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                if (type == REG_DWORD) {
                    return value;
                }
            }
            RegCloseKey(hKey);
        }
        return 0;
    }

    bool WriteRegistryString(HKEY root, const std::string& path, const std::string& valueName, const std::string& value) {
        HKEY hKey;
        std::wstring wpath = StringToWString(path);
        std::wstring wvalueName = StringToWString(valueName);
        std::wstring wvalue = StringToWString(value);

        LONG result = RegCreateKeyExW(root, wpath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (result == ERROR_SUCCESS) {
            result = RegSetValueExW(hKey, wvalueName.c_str(), 0, REG_SZ,
                (const BYTE*)wvalue.c_str(), (DWORD)((wvalue.length() + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
            return result == ERROR_SUCCESS;
        }
        return false;
    }

    bool WriteRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName, DWORD value) {
        HKEY hKey;
        std::wstring wpath = StringToWString(path);
        std::wstring wvalueName = StringToWString(valueName);

        LONG result = RegCreateKeyExW(root, wpath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (result == ERROR_SUCCESS) {
            result = RegSetValueExW(hKey, wvalueName.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
            RegCloseKey(hKey);
            return result == ERROR_SUCCESS;
        }
        return false;
    }

    bool RegistryKeyExists(HKEY root, const std::string& path) {
        HKEY hKey;
        std::wstring wpath = StringToWString(path);

        LONG result = RegOpenKeyExW(root, wpath.c_str(), 0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        return false;
    }

    std::string ExecuteCommand(const std::string& command, int* exitCode) {
        std::string result;
        char buffer[128];

        std::string fullCommand = "cmd.exe /C " + command + " 2>&1";
        FILE* pipe = _popen(fullCommand.c_str(), "r");

        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            int code = _pclose(pipe);
            if (exitCode) {
                *exitCode = code;
            }
        }
        return result;
    }

    bool ExecuteCommandSilent(const std::string& command) {
        int exitCode = 0;
        ExecuteCommand(command, &exitCode);
        return exitCode == 0;
    }

    std::vector<WMIProperty> QueryWMI(const std::string& wmiClass, const std::vector<std::string>& properties) {
        std::vector<WMIProperty> results;

        HRESULT hres;
        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            return results;
        }

        hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

        IWbemLocator* pLoc = NULL;
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
        if (FAILED(hres)) {
            CoUninitialize();
            return results;
        }

        IWbemServices* pSvc = NULL;
        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
        if (FAILED(hres)) {
            pLoc->Release();
            CoUninitialize();
            return results;
        }

        hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        std::string query = "SELECT * FROM " + wmiClass;
        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(query.c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

        if (SUCCEEDED(hres)) {
            IWbemClassObject* pclsObj = NULL;
            ULONG uReturn = 0;

            while (pEnumerator) {
                HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
                if (0 == uReturn) break;

                for (const auto& prop : properties) {
                    VARIANT vtProp;
                    hr = pclsObj->Get(StringToWString(prop).c_str(), 0, &vtProp, 0, 0);
                    if (SUCCEEDED(hr)) {
                        WMIProperty wmiProp;
                        wmiProp.name = prop;
                        if (vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL) {
                            wmiProp.value = WStringToString(std::wstring(vtProp.bstrVal));
                        } else if (vtProp.vt == VT_I4) {
                            wmiProp.value = std::to_string(vtProp.lVal);
                        }
                        results.push_back(wmiProp);
                        VariantClear(&vtProp);
                    }
                }
                pclsObj->Release();
            }
            pEnumerator->Release();
        }

        pSvc->Release();
        pLoc->Release();
        CoUninitialize();

        return results;
    }

    bool RestartNetworkAdapter(const std::string& adapterName) {
        std::string disableCmd = "netsh interface set interface \"" + adapterName + "\" disabled";
        std::string enableCmd = "netsh interface set interface \"" + adapterName + "\" enabled";

        if (!ExecuteCommandSilent(disableCmd)) {
            return false;
        }

        Sleep(2000); // Wait 2 seconds

        return ExecuteCommandSilent(enableCmd);
    }

    bool CreateDirectoryIfNotExists(const std::string& path) {
        std::wstring wpath = StringToWString(path);
        DWORD attrib = GetFileAttributesW(wpath.c_str());

        if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            return true; // Already exists
        }

        return CreateDirectoryW(wpath.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool FileExists(const std::string& path) {
        std::wstring wpath = StringToWString(path);
        DWORD attrib = GetFileAttributesW(wpath.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    std::string GetTimestampString() {
        SYSTEMTIME st;
        GetLocalTime(&st);

        char buffer[64];
        sprintf_s(buffer, "%04d%02d%02d_%02d%02d%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        return std::string(buffer);
    }

    bool IsRunningAsAdministrator() {
        BOOL isAdmin = FALSE;
        PSID administratorsGroup = NULL;

        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup)) {
            CheckTokenMembership(NULL, administratorsGroup, &isAdmin);
            FreeSid(administratorsGroup);
        }

        return isAdmin == TRUE;
    }

    bool EnablePrivilege(const std::string& privilegeName) {
        HANDLE hToken;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            return false;
        }

        TOKEN_PRIVILEGES tp;
        LUID luid;

        if (!LookupPrivilegeValueA(NULL, privilegeName.c_str(), &luid)) {
            CloseHandle(hToken);
            return false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL) != 0;
        CloseHandle(hToken);

        return result;
    }

    std::wstring StringToWString(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
        std::wstring wstr(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size);
        return wstr;
    }

    std::string WStringToString(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string str(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size, NULL, NULL);
        return str;
    }

    std::string ToUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string ToLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

} // namespace Utils
} // namespace HWIDRandomizer
