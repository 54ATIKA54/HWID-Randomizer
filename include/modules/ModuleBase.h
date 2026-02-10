#pragma once

#include <string>
#include <functional>

namespace HWIDRandomizer {

    enum class ModuleStatus {
        SUCCESS = 0,
        WARNING = 1,
        ERROR_NONCRITICAL = 2,
        ERROR_CRITICAL = 3
    };

    using ProgressCallback = std::function<void(float progress, const std::string& status)>;

    class ModuleBase {
    public:
        virtual ~ModuleBase() = default;

        // Execute the randomization operation
        virtual ModuleStatus Execute(bool persistentMode, bool nullIdentifiers, ProgressCallback progressCallback) = 0;

        // Get module name
        virtual std::string GetModuleName() const = 0;

        // Check if reboot is required after this module
        virtual bool RequiresReboot() const { return false; }

    protected:
        bool rebootRequired = false;
    };

} // namespace HWIDRandomizer
