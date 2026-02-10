#pragma once

#include "ModuleBase.h"

namespace HWIDRandomizer {

    class MachineGuidModule : public ModuleBase {
    public:
        MachineGuidModule() = default;
        ~MachineGuidModule() override = default;

        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "Windows MachineGuid"; }

    private:
        bool RandomizeMachineGuid(bool nullIdentifiers);
        bool RandomizeRelatedKeys(bool nullIdentifiers);
    };

} // namespace HWIDRandomizer
