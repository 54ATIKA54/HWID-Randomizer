#pragma once
#include "ModuleBase.h"

namespace HWIDRandomizer {
    class TpmModule : public ModuleBase {
    public:
        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "TPM 2.0 Randomization"; }
    };
}
