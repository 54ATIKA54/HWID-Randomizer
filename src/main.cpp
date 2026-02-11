#include "../include/gui/GuiManager.h"
#include "../include/utils/Logger.h"
#include "../include/utils/Utils.h"
#include <winsock2.h>
#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")

// DirectX11 globals
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize logger
    auto& logger = HWIDRandomizer::Logger::GetInstance();
    std::string logPath = "C:\\HWIDBackup\\operation_log_" + HWIDRandomizer::Utils::GetTimestampString() + ".txt";
    HWIDRandomizer::Utils::CreateDirectoryIfNotExists("C:\\HWIDBackup");
    logger.Initialize(logPath);

    logger.LogInfo("===============================================");
    logger.LogInfo("  HWID Randomizer Research Tool v1.0");
    logger.LogInfo("  FOR LEGITIMATE RESEARCH PURPOSES ONLY");
    logger.LogInfo("===============================================");

    // Check administrator privileges
    if (!HWIDRandomizer::Utils::IsRunningAsAdministrator()) {
        MessageBoxA(NULL,
            "Administrator privileges required.\n\n"
            "Please right-click the executable and select 'Run as administrator'.",
            "Administrator Required", MB_OK | MB_ICONERROR);
        logger.LogError("Application started without administrator privileges");
        return 1;
    }
    logger.LogSuccess("Running with administrator privileges");

    // Check Windows version
    if (!HWIDRandomizer::Utils::IsWindows10()) {
        std::string osVersion = HWIDRandomizer::Utils::GetWindowsVersion();
        MessageBoxA(NULL,
            ("This tool only supports Windows 10.\n\nDetected OS: " + osVersion + "\n\nExiting.").c_str(),
            "Unsupported OS", MB_OK | MB_ICONERROR);
        logger.LogError("Unsupported OS: " + osVersion);
        return 2;
    }
    logger.LogSuccess("Windows 10 detected");

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("HWIDRandomizer"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("HWID Randomizer Research Tool"),
        WS_OVERLAPPEDWINDOW, 100, 100, 1440, 810, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        MessageBoxA(NULL, "Failed to initialize DirectX 11.", "Error", MB_OK | MB_ICONERROR);
        logger.LogError("DirectX 11 initialization failed");
        return 3;
    }
    logger.LogSuccess("DirectX 11 initialized");

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Initialize GUI Manager
    HWIDRandomizer::GuiManager guiManager;
    guiManager.Initialize(g_pd3dDevice, g_pd3dDeviceContext, hwnd);
    logger.LogSuccess("GUI Manager initialized");

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    logger.LogInfo("Entering main loop");

    while (msg.message != WM_QUIT && !guiManager.ShouldClose()) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render GUI
        guiManager.Render();

        // Render ImGui
        ImGui::Render();
        const float clear_color[4] = { 0.17f, 0.17f, 0.17f, 1.00f }; // #2c2c2c
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
    }

    logger.LogInfo("Application shutting down");

    // Cleanup
    guiManager.Shutdown();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    logger.LogSuccess("Shutdown complete");
    logger.Close();

    return 0;
}

// Helper functions to set up Direct3D
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
        &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Window procedure
// Forward declare ImGui Win32 message handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
