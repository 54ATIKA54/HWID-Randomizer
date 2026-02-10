#include "../../include/engine/RandomizationEngine.h"
#include "../../include/modules/AllModules.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"
#include <winsock2.h>
#include <Windows.h>

namespace HWIDRandomizer {

    RandomizationEngine::RandomizationEngine()
        : wasSuccessful(false), requiresReboot(false),
          successCount(0), warningCount(0), errorCount(0) {
        backupManager = std::make_unique<BackupManager>();
    }

    RandomizationEngine::~RandomizationEngine() {
        modules.clear();
    }

    void RandomizationEngine::InitializeModules(MotherboardProfile profile) {
        modules.clear();

        // Add all 12 modules in sequence
        modules.push_back(std::make_unique<SmbiosModule>(profile));
        modules.push_back(std::make_unique<TpmModule>());
        modules.push_back(std::make_unique<SecureBootModule>());
        modules.push_back(std::make_unique<MacAddressModule>());
        modules.push_back(std::make_unique<DiskSerialModule>());
        modules.push_back(std::make_unique<CpuModule>());
        modules.push_back(std::make_unique<GpuModule>());
        modules.push_back(std::make_unique<RamModule>());
        modules.push_back(std::make_unique<MonitorModule>());
        modules.push_back(std::make_unique<EfiModule>());
        modules.push_back(std::make_unique<MachineGuidModule>());
        modules.push_back(std::make_unique<UsbModule>());
    }

    bool RandomizationEngine::PreFlightChecks() {
        auto& logger = Logger::GetInstance();

        // Check 1: Administrator privileges
        if (!Utils::IsRunningAsAdministrator()) {
            logger.LogError("Pre-flight: Administrator privileges required");
            return false;
        }
        logger.LogSuccess("Pre-flight: Administrator privileges confirmed");

        // Check 2: Windows 10 version
        if (!Utils::IsWindows10()) {
            logger.LogError("Pre-flight: This tool only supports Windows 10 (detected: " +
                Utils::GetWindowsVersion() + ")");
            return false;
        }
        logger.LogSuccess("Pre-flight: Windows 10 detected");

        // Check 3: Disk space
        ULARGE_INTEGER freeBytes;
        if (GetDiskFreeSpaceExA("C:\\", &freeBytes, NULL, NULL)) {
            ULONGLONG freeMB = freeBytes.QuadPart / (1024 * 1024);
            if (freeMB < 100) {
                logger.LogWarning("Pre-flight: Low disk space (" + std::to_string(freeMB) + " MB free)");
            } else {
                logger.LogSuccess("Pre-flight: Sufficient disk space (" + std::to_string(freeMB) + " MB free)");
            }
        }

        return true;
    }

    bool RandomizationEngine::CreateBackup(const std::string& timestamp) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("Creating backup: " + timestamp);

        return backupManager->CreateBackup(timestamp);
    }

    bool RandomizationEngine::Execute(const EngineConfig& config, GlobalProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();

        // Initialize counters
        wasSuccessful = false;
        requiresReboot = false;
        successCount = 0;
        warningCount = 0;
        errorCount = 0;

        std::string modeStr = config.persistentMode ? "Persistent" : "Temporary";
        logger.LogInfo("==================================================");
        logger.LogInfo("Starting HWID randomization (" + modeStr + " mode, MSI MAG X670E profile)");
        logger.LogInfo("==================================================");

        // Pre-flight checks
        progressCallback(0.0f, 0, 12, "Running pre-flight checks...");
        if (!PreFlightChecks()) {
            logger.LogError("Pre-flight checks failed. Aborting.");
            return false;
        }

        // Create system restore point
        if (config.createSystemRestorePoint) {
            progressCallback(0.02f, 0, 12, "Creating system restore point...");
            std::string rpName = "HWID Randomizer - " + Utils::GetTimestampString();
            if (!backupManager->CreateSystemRestorePoint(rpName)) {
                logger.LogWarning("System restore point creation failed - continuing with caution");
            }
        }

        // Create backup
        if (config.createBackup) {
            progressCallback(0.04f, 0, 12, "Creating backup...");
            std::string timestamp = Utils::GetTimestampString();
            if (!CreateBackup(timestamp)) {
                logger.LogError("Backup creation failed. Aborting for safety.");
                return false;
            }
        }

        // Initialize modules
        InitializeModules(config.motherboardProfile);
        int totalModules = modules.size();

        // Execute each module
        for (size_t i = 0; i < modules.size(); i++) {
            float baseProgress = 0.05f + (0.95f * static_cast<float>(i) / totalModules);
            std::string moduleName = modules[i]->GetModuleName();

            logger.LogInfo("--------------------------------------------------");
            logger.LogInfo("Module " + std::to_string(i + 1) + "/" + std::to_string(totalModules) + ": " + moduleName);
            logger.LogInfo("--------------------------------------------------");

            progressCallback(baseProgress, i + 1, totalModules, "Executing: " + moduleName);

            // Module progress callback
            auto moduleProgressCallback = [&](float moduleProgress, const std::string& status) {
                float overallProgress = baseProgress + (0.95f / totalModules) * moduleProgress;
                progressCallback(overallProgress, i + 1, totalModules, moduleName + ": " + status);
            };

            ModuleStatus status = modules[i]->Execute(config.persistentMode, config.nullIdentifiers, moduleProgressCallback);

            switch (status) {
                case ModuleStatus::SUCCESS:
                    successCount++;
                    break;
                case ModuleStatus::WARNING:
                    warningCount++;
                    break;
                case ModuleStatus::ERROR_NONCRITICAL:
                    errorCount++;
                    break;
                case ModuleStatus::ERROR_CRITICAL:
                    errorCount++;
                    logger.LogError("Critical error in module: " + moduleName);
                    break;
            }

            if (modules[i]->RequiresReboot()) {
                requiresReboot = true;
            }
        }

        // Final status
        progressCallback(1.0f, totalModules, totalModules, "Randomization complete");

        logger.LogInfo("==================================================");
        logger.LogInfo("Randomization complete!");
        logger.LogInfo("Success: " + std::to_string(successCount) + " | Warnings: " +
            std::to_string(warningCount) + " | Errors: " + std::to_string(errorCount));
        if (requiresReboot) {
            logger.LogWarning("REBOOT REQUIRED for changes to take effect");
        }
        logger.LogInfo("==================================================");

        wasSuccessful = (successCount > 0);
        return wasSuccessful;
    }

    bool RandomizationEngine::RevertToBackup(const std::string& backupTimestamp) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("Reverting to backup: " + backupTimestamp);

        return backupManager->RestoreFromBackup(backupTimestamp);
    }

    std::vector<std::string> RandomizationEngine::GetAvailableBackups() {
        return backupManager->ListAvailableBackups();
    }

} // namespace HWIDRandomizer
