#pragma once

#include "ModuleBase.h"
#include <vector>

namespace HWIDRandomizer {

    struct NetworkAdapter {
        std::string name;
        std::string currentMAC;
        std::string registryPath;
    };

    class MacAddressModule : public ModuleBase {
    public:
        MacAddressModule() = default;
        ~MacAddressModule() override = default;

        ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) override;
        std::string GetModuleName() const override { return "MAC Address Randomization"; }
        bool RequiresReboot() const override { return false; }

    private:
        std::vector<NetworkAdapter> EnumerateNetworkAdapters();
        bool RandomizeMACAddress(const NetworkAdapter& adapter, bool nullIdentifiers);
        bool RestartAdapter(const std::string& adapterName);
    };

} // namespace HWIDRandomizer
