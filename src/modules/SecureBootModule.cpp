#include "../../include/modules/SecureBootModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"

namespace HWIDRandomizer {
    ModuleStatus SecureBootModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("Secure Boot: Starting HVCI configuration");

        progressCallback(0.3f, "Disabling HVCI...");
        Utils::ExecuteCommand("bcdedit /set hypervisorlaunchtype off");

        progressCallback(0.6f, "Updating Core Isolation registry...");
        Utils::WriteRegistryDWORD(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity",
            "Enabled", 0);

        rebootRequired = true;
        logger.LogWarning("Secure Boot: Reboot required for changes to take effect");
        logger.LogSuccess("Secure Boot: HVCI disabled");

        progressCallback(1.0f, "Complete");
        return ModuleStatus::SUCCESS;
    }
}
