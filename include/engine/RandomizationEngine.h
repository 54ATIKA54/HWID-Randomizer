#pragma once

#include "../modules/ModuleBase.h"
#include "../backup/BackupManager.h"
#include <memory>
#include <vector>
#include <functional>

namespace HWIDRandomizer {

    struct EngineConfig {
        bool persistentMode = true;
        bool nullIdentifiers = false;
        bool createSystemRestorePoint = true;
        bool createBackup = true;
        MotherboardProfile motherboardProfile = MotherboardProfile::MSI_MAG_X670E_TOMAHAWK;
    };

    using GlobalProgressCallback = std::function<void(float overallProgress, int currentModule, int totalModules, const std::string& status)>;

    class RandomizationEngine {
    public:
        RandomizationEngine();
        ~RandomizationEngine();

        // Main execution
        bool Execute(const EngineConfig& config, GlobalProgressCallback progressCallback);

        // Revert operations
        bool RevertToBackup(const std::string& backupTimestamp);
        std::vector<std::string> GetAvailableBackups();

        // Status
        bool WasSuccessful() const { return wasSuccessful; }
        bool RequiresReboot() const { return requiresReboot; }
        int GetSuccessCount() const { return successCount; }
        int GetWarningCount() const { return warningCount; }
        int GetErrorCount() const { return errorCount; }

    private:
        std::unique_ptr<BackupManager> backupManager;
        std::vector<std::unique_ptr<ModuleBase>> modules;

        bool wasSuccessful;
        bool requiresReboot;
        int successCount;
        int warningCount;
        int errorCount;

        void InitializeModules(MotherboardProfile profile);
        bool PreFlightChecks();
        bool CreateBackup(const std::string& timestamp);
    };

} // namespace HWIDRandomizer
