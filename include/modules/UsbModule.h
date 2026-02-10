#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class UsbModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "USB Controller Serial Randomization"; }
    };
}
