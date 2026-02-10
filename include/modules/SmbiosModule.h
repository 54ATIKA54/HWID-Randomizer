#pragma once

#include "ModuleBase.h"
#include "../Common.h"
#include <string>

namespace HWIDRandomizer {

    class SmbiosModule : public ModuleBase {
    public:
        SmbiosModule(MotherboardProfile profile);
        ~SmbiosModule() override = default;

        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "SMBIOS / System Information"; }

    private:
        MotherboardProfile profile;

        bool RandomizeBaseBoardSerial(bool nullIdentifiers);
        bool RandomizeBIOSVersion(bool nullIdentifiers);
        bool RandomizeSystemUUID(bool nullIdentifiers);
        bool RandomizeSystemManufacturer();
        bool RandomizeSystemModel();

        std::string GenerateMSISerial();
        std::string GenerateBIOSVersion();
        std::string GenerateBIOSDate();
    };

} // namespace HWIDRandomizer
