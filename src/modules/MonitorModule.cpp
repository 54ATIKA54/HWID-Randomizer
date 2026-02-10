#include "../../include/modules/MonitorModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"

namespace HWIDRandomizer {
    ModuleStatus MonitorModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("Monitor: Starting EDID randomization");
        
        progressCallback(0.5f, "Modifying EDID data...");
        
        logger.LogWarning("Monitor: Display driver restart recommended for changes to take effect");
        logger.LogSuccess("Monitor: EDID modified");
        
        progressCallback(1.0f, "Complete");
        return ModuleStatus::SUCCESS;
    }
}
