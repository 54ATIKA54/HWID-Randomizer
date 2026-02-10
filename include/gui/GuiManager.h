#pragma once

#include "../engine/RandomizationEngine.h"
#include <string>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace HWIDRandomizer {

    class GuiManager {
    public:
        GuiManager();
        ~GuiManager();

        void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, void* hwnd);
        void Shutdown();

        void Render();
        bool ShouldClose() const { return shouldClose; }

    private:
        std::unique_ptr<RandomizationEngine> engine;
        EngineConfig config;

        bool shouldClose;
        bool operationInProgress;
        float currentProgress;
        int currentModule;
        int totalModules;
        std::string currentStatus;

        std::string selectedProfile;
        int selectedProfileIndex;

        bool rebootRequired;

        void RenderHeader();
        void RenderLeftPanel();
        void RenderRightPanel();
        void RenderActionArea();
        void RenderLogConsole();

        void OnApplyButtonClicked();
        void OnRevertButtonClicked();
        void OnExportLogButtonClicked();
        void OnVerifyButtonClicked();

        void ProgressCallback(float progress, int module, int total, const std::string& status);
    };

} // namespace HWIDRandomizer
