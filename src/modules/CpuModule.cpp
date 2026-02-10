#include "../../include/modules/CpuModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"

namespace HWIDRandomizer {
    ModuleStatus CpuModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("CPU: Starting processor name randomization");
        
        progressCallback(0.5f, "Randomizing processor name...");
        
        std::string newName = nullIdentifiers ? "Generic Processor" : 
            "Intel(R) Core(TM) i7-" + std::to_string(8000 + rand() % 2000) + " CPU @ 3.60GHz";
        
        Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", newName);
        
        logger.LogInfo("CPU: CPUID registry cache modified (actual CPUID unchanged)");
        logger.LogSuccess("CPU: Randomization complete");
        
        progressCallback(1.0f, "Complete");
        return ModuleStatus::SUCCESS;
    }
}
