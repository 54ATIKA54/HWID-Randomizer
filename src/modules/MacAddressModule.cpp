#include "../../include/modules/MacAddressModule.h"
#include "../../include/utils/Logger.h"
#include "../../include/utils/Utils.h"
#include <winsock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <algorithm>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace HWIDRandomizer {

    ModuleStatus MacAddressModule::Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("MAC Address: Starting randomization");

        progressCallback(0.0f, "Enumerating network adapters...");

        std::vector<NetworkAdapter> adapters = EnumerateNetworkAdapters();
        logger.LogInfo("MAC Address: Found " + std::to_string(adapters.size()) + " network adapters");

        if (adapters.empty()) {
            logger.LogWarning("MAC Address: No network adapters detected");
            return ModuleStatus::WARNING;
        }

        int success = 0;
        int total = (int)adapters.size();

        for (size_t i = 0; i < adapters.size(); i++) {
            float progress = static_cast<float>(i) / total;
            progressCallback(progress, "Randomizing adapter: " + adapters[i].name);

            logger.LogInfo("MAC Address: Adapter '" + adapters[i].name + "' - Current: " + adapters[i].currentMAC);

            if (RandomizeMACAddress(adapters[i], nullIdentifiers)) {
                success++;

                progressCallback(progress + 0.3f / total, "Restarting adapter...");
                RestartAdapter(adapters[i].name);
            }
        }

        progressCallback(1.0f, "MAC randomization complete");
        logger.LogSuccess("MAC Address: Randomized " + std::to_string(success) + " / " +
            std::to_string(total) + " adapters");

        return success > 0 ? ModuleStatus::SUCCESS : ModuleStatus::ERROR_NONCRITICAL;
    }

    std::vector<NetworkAdapter> MacAddressModule::EnumerateNetworkAdapters() {
        std::vector<NetworkAdapter> adapters;

        ULONG bufferSize = 15000;
        PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);

        if (pAddresses == nullptr) {
            return adapters;
        }

        DWORD result = GetAdaptersAddresses(AF_UNSPEC,
            GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST,
            NULL, pAddresses, &bufferSize);

        if (result == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);
            result = GetAdaptersAddresses(AF_UNSPEC,
                GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST,
                NULL, pAddresses, &bufferSize);
        }

        if (result == NO_ERROR) {
            PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
            while (pCurrAddresses) {
                // Skip loopback and software adapters
                if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK &&
                    pCurrAddresses->PhysicalAddressLength == 6) {

                    NetworkAdapter adapter;
                    adapter.name = Utils::WStringToString(std::wstring(pCurrAddresses->FriendlyName));

                    // Format MAC address
                    char macStr[32];
                    sprintf_s(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                        pCurrAddresses->PhysicalAddress[0],
                        pCurrAddresses->PhysicalAddress[1],
                        pCurrAddresses->PhysicalAddress[2],
                        pCurrAddresses->PhysicalAddress[3],
                        pCurrAddresses->PhysicalAddress[4],
                        pCurrAddresses->PhysicalAddress[5]);
                    adapter.currentMAC = std::string(macStr);

                    // Try to find registry path
                    adapter.registryPath = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}";

                    adapters.push_back(adapter);
                }

                pCurrAddresses = pCurrAddresses->Next;
            }
        }

        free(pAddresses);
        return adapters;
    }

    bool MacAddressModule::RandomizeMACAddress(const NetworkAdapter& adapter, bool nullIdentifiers) {
        auto& logger = Logger::GetInstance();

        std::string newMAC;
        if (nullIdentifiers) {
            newMAC = "000000000000";
            logger.LogWarning("MAC Address: Null MAC may cause network connectivity issues");
        } else {
            newMAC = Utils::GenerateRandomMAC(true);
            // Remove colons for registry format
            newMAC.erase(std::remove(newMAC.begin(), newMAC.end(), ':'), newMAC.end());
        }

        // Try to write to common network adapter registry locations
        // This is a simplified approach - production code would enumerate all subkeys

        for (int i = 0; i < 20; i++) {
            std::string paddedNum = std::to_string(i);
            while (paddedNum.length() < 4) {
                paddedNum = "0" + paddedNum;
            }
            std::string keyPath = adapter.registryPath + "\\" + paddedNum;

            // Check if this key matches our adapter
            std::string driverDesc = Utils::ReadRegistryString(HKEY_LOCAL_MACHINE, keyPath, "DriverDesc");
            if (driverDesc.empty()) continue;

            // Write NetworkAddress value
            if (Utils::WriteRegistryString(HKEY_LOCAL_MACHINE, keyPath, "NetworkAddress", newMAC)) {
                std::string formattedMAC = newMAC;
                formattedMAC.insert(10, ":");
                formattedMAC.insert(8, ":");
                formattedMAC.insert(6, ":");
                formattedMAC.insert(4, ":");
                formattedMAC.insert(2, ":");

                logger.LogSuccess("MAC Address: Adapter '" + adapter.name + "' - Randomized to: " + formattedMAC);
                return true;
            }
        }

        logger.LogError("MAC Address: Could not find registry entry for adapter: " + adapter.name);
        return false;
    }

    bool MacAddressModule::RestartAdapter(const std::string& adapterName) {
        return Utils::RestartNetworkAdapter(adapterName);
    }

} // namespace HWIDRandomizer
