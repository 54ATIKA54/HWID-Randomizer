# HWID Randomizer - Internal API Reference

**Target Audience:** Developers extending or modifying the tool

---

## Project Structure

```
HWIDRandomizer/
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── utils/                      # Utility functions
│   │   ├── Logger.{cpp,h}          # Logging system
│   │   └── Utils.{cpp,h}           # Helper functions
│   ├── backup/                     # Backup management
│   │   └── BackupManager.{cpp,h}
│   ├── engine/                     # Core engine
│   │   └── RandomizationEngine.{cpp,h}
│   ├── gui/                        # ImGui interface
│   │   └── GuiManager.{cpp,h}
│   └── modules/                    # 12 randomization modules
│       ├── ModuleBase.h            # Base interface
│       ├── SmbiosModule.{cpp,h}
│       ├── TpmModule.{cpp,h}
│       ├── MacAddressModule.{cpp,h}
│       └── ... (9 more modules)
└── include/                        # Header files mirroring src/
```

---

## Core Classes

### Logger

**Namespace:** `HWIDRandomizer`
**File:** `utils/Logger.{cpp,h}`
**Purpose:** Centralized logging to file and GUI

#### Methods

```cpp
static Logger& GetInstance();
```
Singleton instance access

```cpp
void Initialize(const std::string& logFilePath);
```
Initialize logger with file path

```cpp
void Log(LogLevel level, const std::string& message);
void LogInfo(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);
void LogSuccess(const std::string& message);
```
Log messages with different severity levels

```cpp
const std::vector<std::string>& GetGuiMessages() const;
```
Retrieve last 100 messages for GUI display

```cpp
void Close();
```
Close log file and cleanup

#### Log Levels

```cpp
enum class LogLevel {
    INFO,     // Informational
    WARNING,  // Non-critical issues
    ERROR,    // Critical errors
    SUCCESS   // Successful operations
};
```

---

### Utils

**Namespace:** `HWIDRandomizer::Utils`
**File:** `utils/Utils.{cpp,h}`
**Purpose:** Helper functions for Windows APIs, random generation

#### Random Generation

```cpp
std::string GenerateRandomMAC(bool useValidOUI = true);
```
Generate random MAC address with valid locally-administered OUI

```cpp
std::string GenerateRandomSerial(int length, const std::string& prefix = "");
```
Generate random alphanumeric serial with optional prefix

```cpp
std::string GenerateRandomGUID();
```
Generate Windows GUID in format `{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}`

```cpp
std::string GenerateRandomHex(int bytes);
```
Generate random hexadecimal string

#### OS Detection

```cpp
bool IsWindows10();
```
Check if running on Windows 10 (not Windows 11)

```cpp
std::string GetWindowsVersion();
```
Get Windows version string

#### Registry Operations

```cpp
std::string ReadRegistryString(HKEY root, const std::string& path, const std::string& valueName);
DWORD ReadRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName);
bool WriteRegistryString(HKEY root, const std::string& path, const std::string& valueName, const std::string& value);
bool WriteRegistryDWORD(HKEY root, const std::string& path, const std::string& valueName, DWORD value);
bool RegistryKeyExists(HKEY root, const std::string& path);
```

#### Command Execution

```cpp
std::string ExecuteCommand(const std::string& command, int* exitCode = nullptr);
bool ExecuteCommandSilent(const std::string& command);
```

#### WMI Operations

```cpp
struct WMIProperty {
    std::string name;
    std::string value;
};

std::vector<WMIProperty> QueryWMI(const std::string& wmiClass, const std::vector<std::string>& properties);
```

#### Network Operations

```cpp
bool RestartNetworkAdapter(const std::string& adapterName);
```

#### File Operations

```cpp
bool CreateDirectoryIfNotExists(const std::string& path);
bool FileExists(const std::string& path);
std::string GetTimestampString();  // Format: YYYYMMDD_HHMMSS
```

#### Privileges

```cpp
bool IsRunningAsAdministrator();
bool EnablePrivilege(const std::string& privilegeName);
```

#### String Utilities

```cpp
std::wstring StringToWString(const std::string& str);
std::string WStringToString(const std::wstring& wstr);
std::string ToUpper(const std::string& str);
std::string ToLower(const std::string& str);
```

---

### BackupManager

**Namespace:** `HWIDRandomizer`
**File:** `backup/BackupManager.{cpp,h}`
**Purpose:** Create and restore system backups

#### Methods

```cpp
BackupManager();
```
Constructor - initializes backup directory `C:\HWIDBackup`

```cpp
bool CreateBackup(const std::string& timestamp);
```
Create backup with given timestamp (format: YYYYMMDD_HHMMSS)

```cpp
bool CreateSystemRestorePoint(const std::string& description);
```
Create Windows System Restore point

```cpp
void StoreOriginalValue(const std::string& category, const std::string& identifier, const std::string& value);
```
Store original value before modification

```cpp
bool SaveBackupToDisk(const std::string& backupPath);
bool SaveBackupSummary(const std::string& summaryPath);
bool ExportRegistryBackup(const std::string& regPath);
```
Save backup files

```cpp
bool RestoreFromBackup(const std::string& backupTimestamp);
```
Restore from backup by timestamp

```cpp
std::vector<std::string> ListAvailableBackups();
```
List all available backup timestamps

---

### Module Base Interface

**Namespace:** `HWIDRandomizer`
**File:** `modules/ModuleBase.h`
**Purpose:** Base class for all randomization modules

#### Interface

```cpp
class ModuleBase {
public:
    virtual ~ModuleBase() = default;

    // Execute the randomization operation
    virtual ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) = 0;

    // Get module name
    virtual std::string GetModuleName() const = 0;

    // Check if reboot is required after this module
    virtual bool RequiresReboot() const { return false; }
};
```

#### Module Status

```cpp
enum class ModuleStatus {
    SUCCESS = 0,              // Operation succeeded
    WARNING = 1,              // Succeeded with warnings
    ERROR_NONCRITICAL = 2,    // Failed but continue
    ERROR_CRITICAL = 3        // Critical failure
};
```

#### Progress Callback

```cpp
using ProgressCallback = std::function<void(float progress, const std::string& status)>;
```

---

### RandomizationEngine

**Namespace:** `HWIDRandomizer`
**File:** `engine/RandomizationEngine.{cpp,h}`
**Purpose:** Orchestrate all randomization modules

#### Configuration

```cpp
struct EngineConfig {
    bool persistentMode;                  // Persistent vs temporary
    bool nullIdentifiers;                 // Use null/zero values
    bool createSystemRestorePoint;        // Create restore point
    bool createBackup;                    // Create backup
    MotherboardProfile motherboardProfile; // Selected profile
};
```

#### Callback

```cpp
using GlobalProgressCallback = std::function<void(float overallProgress, int currentModule, int totalModules, const std::string& status)>;
```

#### Methods

```cpp
RandomizationEngine();
```
Constructor - initializes backup manager

```cpp
bool Execute(const EngineConfig& config, GlobalProgressCallback progressCallback);
```
Execute full randomization with configuration and progress reporting

```cpp
bool RevertToBackup(const std::string& backupTimestamp);
```
Revert to backup

```cpp
std::vector<std::string> GetAvailableBackups();
```
List available backups

#### Status Getters

```cpp
bool WasSuccessful() const;
bool RequiresReboot() const;
int GetSuccessCount() const;
int GetWarningCount() const;
int GetErrorCount() const;
```

---

## Command Line Parameters

### /persist-reapply

**Purpose:** Silent reapply of last configuration (used by scheduled task)

**Behavior:**
- Loads configuration from `C:\HWIDBackup\last_config.json`
- Executes randomization without GUI
- Logs to `C:\HWIDBackup\persist_log_YYYYMMDD.txt`
- Exits after completion

### /revert-from-backup `<timestamp>`

**Purpose:** Automatic revert from backup (used by temporary mode cleanup)

**Behavior:**
- Restores specified backup
- Logs to standard log file
- Exits after completion

---

## Configuration File Format

### last_config.json

**Location:** `C:\HWIDBackup\last_config.json`

**Format:**
```json
{
    "mode": "persistent",
    "profile": "MSI MAG X670E TOMAHAWK WIFI",
    "null_identifiers": false,
    "randomized_values": {
        "mac_addresses": {
            "Ethernet": "02:A4:7F:89:1E:5D"
        },
        "machine_guid": "{9B2C1F7E-8A4D-3C6B-2E1F-7D9A3B8C4E5F}",
        "smbios_serial": "MS7E12XY789ABC",
        "bios_version": "A.67",
        "system_uuid": "{3F8A2B4D-1C5E-4F7A-9B2C-8D6E4A3F1B5C}"
    }
}
```

---

## Module Implementation Template

To create a new randomization module:

```cpp
// MyModule.h
#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class MyModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "My Custom Module"; }
        bool RequiresReboot() const override { return false; }
    };
}

// MyModule.cpp
#include "MyModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"

namespace HWIDRandomizer {
    ModuleStatus MyModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("MyModule: Starting operation");

        progressCallback(0.0f, "Initializing...");

        // Perform operations here

        progressCallback(1.0f, "Complete");
        logger.LogSuccess("MyModule: Operation complete");

        return ModuleStatus::SUCCESS;
    }
}
```

Add to `RandomizationEngine::InitializeModules()`:
```cpp
modules.push_back(std::make_unique<MyModule>());
```

---

## Error Handling Convention

1. **Never throw exceptions** - Use return codes
2. **Log all errors** - Use Logger for visibility
3. **Continue on non-critical errors** - Set warning status
4. **Provide descriptive messages** - Include error codes when available
5. **Check prerequisites** - Validate before operations

---

## Registry Key Reference

### SMBIOS / System Info

```
HKLM\HARDWARE\DESCRIPTION\System\BIOS
├── SystemManufacturer
├── SystemProductName
├── BaseBoardProduct
├── BaseBoardManufacturer
├── BaseBoardSerialNumber
├── BIOSVersion
├── BIOSReleaseDate
└── SystemUUID
```

### CPU

```
HKLM\HARDWARE\DESCRIPTION\System\CentralProcessor\0
├── ProcessorNameString
├── Identifier
└── VendorIdentifier
```

### MachineGuid

```
HKLM\SOFTWARE\Microsoft\Cryptography
└── MachineGuid

HKLM\SOFTWARE\Microsoft\SQMClient
└── MachineId

HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate
└── SusClientId
```

### Network Adapters

```
HKLM\SYSTEM\CurrentControlSet\Control\Class\{4D36E972-E325-11CE-BFC1-08002BE10318}\XXXX
└── NetworkAddress
```

---

## Building Documentation

This documentation is manually maintained. When adding new features:

1. Update this API reference
2. Update README.md usage section
3. Update USAGE.md if workflow changes
4. Add examples to docs/examples/ (if created)

---

**For detailed implementation, refer to source code comments and planning.md specification.**
