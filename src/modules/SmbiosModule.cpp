#include "../../include/modules/SmbiosModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"
#include <Windows.h>

namespace HWIDRandomizer {

    SmbiosModule::SmbiosModule(MotherboardProfile profile) : profile(profile) {}

    ModuleStatus SmbiosModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("SMBIOS: Starting randomization (Profile: MSI MAG X670E)");

        progressCallback(0.0f, "Reading current SMBIOS values...");

        // Read current values for backup
        std::string currentSerial = Utils::ReadRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardSerialNumber");
        std::string currentBIOSVersion = Utils::ReadRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSVersion");

        logger.LogInfo("SMBIOS: Current baseboard serial: " + (currentSerial.empty() ? "(empty)" : currentSerial));

        progressCallback(0.2f, "Randomizing baseboard serial...");
        if (!RandomizeBaseBoardSerial(nullIdentifiers)) {
            logger.LogWarning("SMBIOS: Failed to randomize baseboard serial");
        }

        progressCallback(0.4f, "Randomizing BIOS version...");
        if (!RandomizeBIOSVersion(nullIdentifiers)) {
            logger.LogWarning("SMBIOS: Failed to randomize BIOS version");
        }

        progressCallback(0.6f, "Randomizing system UUID...");
        if (!RandomizeSystemUUID(nullIdentifiers)) {
            logger.LogWarning("SMBIOS: Failed to randomize system UUID");
        }

        progressCallback(0.8f, "Randomizing system info...");
        RandomizeSystemManufacturer();
        RandomizeSystemModel();

        progressCallback(1.0f, "SMBIOS randomization complete");
        logger.LogSuccess("SMBIOS: Randomization complete");

        return ModuleStatus::SUCCESS;
    }

    bool SmbiosModule::RandomizeBaseBoardSerial(bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newSerial;
        if (nullIdentifiers) {
            newSerial = "00000000";
        } else {
            newSerial = GenerateMSISerial();
        }

        bool success = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardSerialNumber", newSerial);

        if (success) {
            logger.LogSuccess("SMBIOS: Baseboard serial randomized to: " + newSerial);
        } else {
            logger.LogError("SMBIOS: Failed to write baseboard serial");
        }

        return success;
    }

    bool SmbiosModule::RandomizeBIOSVersion(bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newVersion;
        if (nullIdentifiers) {
            newVersion = "A.00";
        } else {
            newVersion = GenerateBIOSVersion();
        }

        bool success = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSVersion", newVersion);

        if (success) {
            logger.LogSuccess("SMBIOS: BIOS version randomized to: " + newVersion);
        } else {
            logger.LogError("SMBIOS: Failed to write BIOS version");
        }

        // Also randomize BIOS date
        std::string newDate = GenerateBIOSDate();
        Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSReleaseDate", newDate);

        return success;
    }

    bool SmbiosModule::RandomizeSystemUUID(bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newUUID;
        if (nullIdentifiers) {
            newUUID = "{00000000-0000-0000-0000-000000000000}";
        } else {
            newUUID = Utils::GenerateRandomGUID();
        }

        bool success = Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "SystemUUID", newUUID);

        if (success) {
            logger.LogSuccess("SMBIOS: System UUID randomized");
        } else {
            logger.LogError("SMBIOS: Failed to write system UUID");
        }

        return success;
    }

    bool SmbiosModule::RandomizeSystemManufacturer() {
        std::string manufacturer = "Micro-Star International Co., Ltd.";

        return Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "SystemManufacturer", manufacturer);
    }

    bool SmbiosModule::RandomizeSystemModel() {
        std::string model = "MS-7E12";

        Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "SystemProductName", "MAG X670E TOMAHAWK WIFI (MS-7E12)");

        return Utils::WriteRegistryString(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardProduct", "MAG X670E TOMAHAWK WIFI (MS-7E12)");
    }

    std::string SmbiosModule::GenerateMSISerial() {
        return Utils::GenerateRandomSerial(8, "MS7E12");
    }

    std::string SmbiosModule::GenerateBIOSVersion() {
        int version = 10 + (rand() % 90); // Random version between 10-99
        return "A." + std::to_string(version);
    }

    std::string SmbiosModule::GenerateBIOSDate() {
        int year = 2022 + (rand() % 3); // 2022-2024
        int month = 1 + (rand() % 12);
        int day = 1 + (rand() % 28);

        char buffer[32];
        sprintf_s(buffer, "%02d/%02d/%04d", month, day, year);
        return std::string(buffer);
    }

} // namespace HWIDRandomizer
