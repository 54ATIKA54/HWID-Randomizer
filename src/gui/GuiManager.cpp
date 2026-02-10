#include "../../include/gui/GuiManager.h"
#include "../../include/utils/Logger.h"
#include <winsock2.h>
#include <Windows.h>

// ImGui headers (will be included when ImGui is added)
// For now, stub implementation
namespace ImGui {
    void Begin(const char*, bool* = nullptr, int = 0) {}
    void End() {}
    void Text(const char*, ...) {}
    void TextColored(int, const char*, ...) {}
    bool Button(const char*, int = 0, int = 0) { return false; }
    bool RadioButton(const char*, bool) { return false; }
    bool Checkbox(const char*, bool*) { return false; }
    bool Combo(const char*, int*, const char**, int) { return false; }
    void ProgressBar(float, int = 0, int = 0, const char* = nullptr) {}
    void Separator() {}
    void SameLine() {}
    void PushStyleColor(int, int) {}
    void PopStyleColor(int = 1) {}
    void SetNextWindowSize(int, int, int = 0) {}
    void SetNextWindowPos(int, int, int = 0) {}
}

namespace HWIDRandomizer {

    GuiManager::GuiManager()
        : shouldClose(false), operationInProgress(false),
          currentProgress(0.0f), currentModule(0), totalModules(12),
          currentStatus("Ready"), selectedProfileIndex(0), rebootRequired(false) {
        engine = std::make_unique<RandomizationEngine>();

        // Default config
        config.persistentMode = true;
        config.nullIdentifiers = false;
        config.createSystemRestorePoint = true;
        config.createBackup = true;
        config.motherboardProfile = MotherboardProfile::MSI_MAG_X670E_TOMAHAWK;
    }

    GuiManager::~GuiManager() {}

    void GuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, void* hwnd) {
        // Initialize ImGui context
        // ImGui::CreateContext();
        // ImGui_ImplWin32_Init(hwnd);
        // ImGui_ImplDX11_Init(device, context);

        // Setup dark theme colors
        // ImGuiStyle& style = ImGui::GetStyle();
        // ... color configuration matching planning.md ...
    }

    void GuiManager::Shutdown() {
        // ImGui_ImplDX11_Shutdown();
        // ImGui_ImplWin32_Shutdown();
        // ImGui::DestroyContext();
    }

    void GuiManager::Render() {
        // Main window
        ImGui::SetNextWindowSize(1440, 810, 0);
        ImGui::SetNextWindowPos(100, 100, 0);

        ImGui::Begin("HWID Randomizer Research Tool", nullptr, 0);

        RenderHeader();
        RenderLeftPanel();
        RenderRightPanel();
        RenderActionArea();
        RenderLogConsole();

        ImGui::End();
    }

    void GuiManager::RenderHeader() {
        ImGui::Text("HWID Randomizer Research Tool");
        ImGui::SameLine();
        ImGui::TextColored(0x00FF00, "[ADMINISTRATOR]");
        ImGui::Separator();
    }

    void GuiManager::RenderLeftPanel() {
        ImGui::Text("Operation Mode:");
        if (ImGui::RadioButton("Temporary (Until Reboot)", !config.persistentMode)) {
            config.persistentMode = false;
        }
        if (ImGui::RadioButton("Persistent (Survives Reboot)", config.persistentMode)) {
            config.persistentMode = true;
        }

        ImGui::Separator();

        ImGui::Text("Motherboard Profile:");
        const char* profiles[] = {
            "MSI MAG X670E TOMAHAWK WIFI",
            "MSI X670E Generic",
            "ASUS Generic",
            "Generic"
        };
        ImGui::Combo("Profile", &selectedProfileIndex, profiles, 4);

        ImGui::Separator();

        ImGui::Text("Safety Options:");
        ImGui::Checkbox("System Restore Point", &config.createSystemRestorePoint);
        ImGui::Checkbox("Backup Original Values", &config.createBackup);
        ImGui::Checkbox("Null Identifiers", &config.nullIdentifiers);
    }

    void GuiManager::RenderRightPanel() {
        ImGui::Text("Randomization Targets:");
        ImGui::Separator();

        // 12 target cards in 2-column grid
        const char* targets[] = {
            "SMBIOS / System Info", "TPM 2.0", "Secure Boot / HVCI",
            "MAC Address", "Disk / Volume Serial", "CPU Identifier",
            "GPU Identifier", "RAM Modules", "Monitor EDID",
            "EFI Variables", "Windows MachineGuid", "USB Controllers"
        };

        for (int i = 0; i < 12; i++) {
            ImGui::Text("[X] %s", targets[i]);
            if (i % 2 == 0 && i < 11) ImGui::SameLine();
        }
    }

    void GuiManager::RenderActionArea() {
        ImGui::Separator();

        if (operationInProgress) {
            ImGui::Button("PROCESSING...", 0, 0);
        } else {
            if (ImGui::Button("APPLY FULL RANDOMIZATION", 0, 0)) {
                OnApplyButtonClicked();
            }
        }

        ImGui::ProgressBar(currentProgress, 0, 0, nullptr);
        ImGui::Text("Status: %s", currentStatus.c_str());

        if (ImGui::Button("FULL REVERT", 0, 0)) {
            OnRevertButtonClicked();
        }
        ImGui::SameLine();
        if (ImGui::Button("Export Log", 0, 0)) {
            OnExportLogButtonClicked();
        }
        ImGui::SameLine();
        if (ImGui::Button("Verify Changes", 0, 0)) {
            OnVerifyButtonClicked();
        }
    }

    void GuiManager::RenderLogConsole() {
        ImGui::Separator();
        ImGui::Text("Console Log:");

        ImGui::PushStyleColor(0, 0x000000); // Black background
        // Render last 8 log lines from Logger
        auto& logger = Logger::GetInstance();
        const auto& messages = logger.GetGuiMessages();

        int startIdx = (messages.size() > 8) ? messages.size() - 8 : 0;
        for (size_t i = startIdx; i < messages.size(); i++) {
            ImGui::TextColored(0x00FF00, "%s", messages[i].c_str());
        }

        ImGui::PopStyleColor();
    }

    void GuiManager::OnApplyButtonClicked() {
        auto& logger = Logger::GetInstance();
        logger.LogInfo("User initiated full randomization");

        operationInProgress = true;
        currentProgress = 0.0f;

        // Execute in separate thread (simplified - should use std::thread)
        auto progressCb = [this](float progress, int module, int total, const std::string& status) {
            ProgressCallback(progress, module, total, status);
        };

        bool success = engine->Execute(config, progressCb);

        operationInProgress = false;
        rebootRequired = engine->RequiresReboot();

        if (success) {
            MessageBoxA(NULL, "Randomization complete!\n\nCheck log for details.", "Success", MB_OK | MB_ICONINFORMATION);

            if (rebootRequired) {
                int result = MessageBoxA(NULL,
                    "A system reboot is required for all changes to take effect.\n\nReboot now?",
                    "Reboot Required", MB_YESNO | MB_ICONQUESTION);

                if (result == IDYES) {
                    // Initiate reboot
                    system("shutdown /r /t 10 /c \"HWID Randomizer: Reboot required for changes\"");
                }
            }
        } else {
            MessageBoxA(NULL, "Operation completed with errors.\n\nCheck log for details.", "Errors Occurred", MB_OK | MB_ICONWARNING);
        }
    }

    void GuiManager::OnRevertButtonClicked() {
        auto backups = engine->GetAvailableBackups();

        if (backups.empty()) {
            MessageBoxA(NULL, "No backups found.", "Revert", MB_OK | MB_ICONINFORMATION);
            return;
        }

        // In production, show backup selection dialog
        // For now, use the most recent
        std::string mostRecent = backups[backups.size() - 1];

        int result = MessageBoxA(NULL,
            ("Restore from backup: " + mostRecent + "?\n\nThis will revert all changes.").c_str(),
            "Confirm Restore", MB_YESNO | MB_ICONQUESTION);

        if (result == IDYES) {
            if (engine->RevertToBackup(mostRecent)) {
                MessageBoxA(NULL, "Restore complete.\n\nPlease reboot to finalize.", "Success", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBoxA(NULL, "Restore failed.\n\nCheck logs for details.", "Error", MB_OK | MB_ICONERROR);
            }
        }
    }

    void GuiManager::OnExportLogButtonClicked() {
        // Save log to user-selected location
        MessageBoxA(NULL, "Log export functionality (file dialog would appear here)", "Export Log", MB_OK | MB_ICONINFORMATION);
    }

    void GuiManager::OnVerifyButtonClicked() {
        MessageBoxA(NULL, "Verification functionality (would show before/after comparison)", "Verify", MB_OK | MB_ICONINFORMATION);
    }

    void GuiManager::ProgressCallback(float progress, int module, int total, const std::string& status) {
        currentProgress = progress;
        currentModule = module;
        totalModules = total;
        currentStatus = status;
    }

} // namespace HWIDRandomizer
