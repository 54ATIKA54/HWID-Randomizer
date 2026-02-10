#include "../../include/modules/MachineGuidModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"
#include <winsock2.h>
#include <Windows.h>

namespace HWIDRandomizer {

    ModuleStatus MachineGuidModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("MachineGuid: Starting randomization");

        progressCallback(0.0f, "Reading current MachineGuid...");

        std::string currentGuid = Utils::ReadRegistryString(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Cryptography", "MachineGuid");

        if (!currentGuid.empty()) {
            logger.LogInfo("MachineGuid: Current GUID: " + currentGuid);
        }

        progressCallback(0.3f, "Generating new MachineGuid...");
        if (!RandomizeMachineGuid(nullIdentifiers)) {
            logger.LogError("MachineGuid: Failed to randomize primary GUID");
            return ModuleStatus::ERROR_NONCRITICAL;
        }

        progressCallback(0.7f, "Randomizing related keys...");
        RandomizeRelatedKeys(nullIdentifiers);

        progressCallback(1.0f, "MachineGuid randomization complete");
        logger.LogSuccess("MachineGuid: Randomization complete");

        return ModuleStatus::SUCCESS;
    }

    bool MachineGuidModule::RandomizeMachineGuid(bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newGuid;
        if (nullIdentifiers) {
            newGuid = "{00000000-0000-0000-0000-000000000000}";
        } else {
            newGuid = Utils::GenerateRandomGUID();
        }

        bool success = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Cryptography", "MachineGuid", newGuid);

        if (success) {
            logger.LogSuccess("MachineGuid: Randomized to: " + newGuid);
        } else {
            logger.LogError("MachineGuid: Failed to write registry value");
        }

        return success;
    }

    bool MachineGuidModule::RandomizeRelatedKeys(bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newGuid;
        if (nullIdentifiers) {
            newGuid = "{00000000-0000-0000-0000-000000000000}";
        } else {
            newGuid = Utils::GenerateRandomGUID();
        }

        // SQMClient MachineId
        bool success1 = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\SQMClient", "MachineId", newGuid);

        if (success1) {
            logger.LogSuccess("MachineGuid: Updated SQMClient MachineId");
        }

        // Windows Update SusClientId
        std::string newGuid2 = nullIdentifiers ? "{00000000-0000-0000-0000-000000000000}" : Utils::GenerateRandomGUID();
        bool success2 = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", "SusClientId", newGuid2);

        if (success2) {
            logger.LogSuccess("MachineGuid: Updated WindowsUpdate SusClientId");
        }

        return success1 || success2;
    }

} // namespace HWIDRandomizer
