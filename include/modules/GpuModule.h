#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class GpuModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "GPU Identifier Spoofing"; }
    };
}
