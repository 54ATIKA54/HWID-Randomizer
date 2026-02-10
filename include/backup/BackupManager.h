#pragma once

#include <string>
#include <map>
#include <vector>

namespace HWIDRandomizer {

    struct BackupEntry {
        std::string key;
        std::string value;
        std::string originalData;
    };

    class BackupManager {
    public:
        BackupManager();
        ~BackupManager();

        // Backup operations
        bool CreateBackup(const std::string& timestamp);
        bool CreateSystemRestorePoint(const std::string& description);

        // Store values before modification
        void StoreOriginalValue(const std::string& category, const std::string& identifier, const std::string& value);

        // Save backup to disk
        bool SaveBackupToDisk(const std::string& backupPath);
        bool SaveBackupSummary(const std::string& summaryPath);
        bool ExportRegistryBackup(const std::string& regPath);

        // Restore operations
        bool RestoreFromBackup(const std::string& backupTimestamp);
        std::vector<std::string> ListAvailableBackups();

        // Getters
        const std::map<std::string, std::map<std::string, std::string>>& GetBackupData() const { return backupData; }

    private:
        std::map<std::string, std::map<std::string, std::string>> backupData;
        // Category -> (Identifier -> OriginalValue)
        // Example: "SMBIOS" -> ("BaseBoardSerial" -> "MS7E12AB123456")

        std::string backupDirectory;
        std::vector<std::string> registryKeysToBackup;

        void InitializeBackupDirectory();
        void InitializeRegistryKeysList();
    };

} // namespace HWIDRandomizer
