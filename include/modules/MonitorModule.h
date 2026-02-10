#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class MonitorModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "Monitor EDID Randomization"; }
    };
}
