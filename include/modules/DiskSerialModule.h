#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class DiskSerialModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "Disk / Volume Serial"; }
    };
}
