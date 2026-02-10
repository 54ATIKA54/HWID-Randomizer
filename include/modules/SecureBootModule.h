#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class SecureBootModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "Secure Boot / HVCI"; }
        bool RequiresReboot() const override { return true; }
    };
}
