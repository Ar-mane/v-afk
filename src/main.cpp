#include <d3d11.h>
#include <windows.h>
#include <windowsx.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "app_icon.h"
#include "bot.h"

#pragma comment(lib, "d3d11.lib")

static ID3D11Device *g_pd3dDevice = nullptr;
static ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
static IDXGISwapChain *g_pSwapChain = nullptr;
static ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
static ImFont *g_fontUi = nullptr;
static ImFont *g_fontTitle = nullptr;
static ImFont *g_fontMono = nullptr;

Bot g_bot;

// ======================================================
// DX11
// ======================================================

void CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer = nullptr;

    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);

    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();

        g_mainRenderTargetView = nullptr;
    }
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};

    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;

    const D3D_FEATURE_LEVEL featureLevelArray[1] = {D3D_FEATURE_LEVEL_11_0};

    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
                                      featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
                                      &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
    {
        return false;
    }

    CreateRenderTarget();

    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if (g_pSwapChain)
    {
        g_pSwapChain->Release();

        g_pSwapChain = nullptr;
    }

    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();

        g_pd3dDeviceContext = nullptr;
    }

    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();

        g_pd3dDevice = nullptr;
    }
}

// ======================================================
// Win32
// ======================================================

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);

        if (hit == HTCLIENT)
        {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hWnd, &pt);

            const int topBarHeight = 44;
            const int closeButtonLeft = 388;

            if (pt.y >= 0 && pt.y < topBarHeight && pt.x < closeButtonLeft)
            {
                return HTCAPTION;
            }
        }

        return hit;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);

        return 0;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ======================================================
// Styling
// ======================================================

void SetupStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowRounding = 0.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);

    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.06f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.24f, 0.24f, 0.28f, 1.0f);

    colors[ImGuiCol_Text] = ImVec4(0.93f, 0.93f, 0.95f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.56f, 0.56f, 0.60f, 1.0f);

    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.19f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.16f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.19f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.13f, 0.16f, 1.0f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.14f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.14f, 0.14f, 0.18f, 1.0f);
}

void SetupFonts()
{
    ImGuiIO &io = ImGui::GetIO();

    ImFontConfig fontConfig = {};
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;

    g_fontTitle =
        io.Fonts->AddFontFromFileTTF("assets/fonts/Orbitron/Orbitron-Bold.ttf", 17.5f, &fontConfig);

    g_fontUi = io.Fonts->AddFontFromFileTTF("assets/fonts/Orbitron/Orbitron-Medium.ttf", 13.0f,
                                            &fontConfig);

    g_fontMono = io.Fonts->AddFontFromFileTTF("assets/fonts/Fira_Code/FiraCode-Medium.ttf", 14.0f,
                                              &fontConfig);

    if (!g_fontUi)
    {
        g_fontUi = io.Fonts->AddFontDefault();
    }

    if (!g_fontTitle)
    {
        g_fontTitle = g_fontUi;
    }

    if (!g_fontMono)
    {
        g_fontMono = g_fontUi;
    }
}

// ======================================================
// Main
// ======================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    HICON appIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 0, 0,
                                     LR_DEFAULTSIZE);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),          CS_CLASSDC, WndProc, 0L,       0L,     hInstance, appIcon,
        LoadCursor(NULL, IDC_ARROW), NULL,       NULL,    L"V-AFK", appIcon};

    RegisterClassEx(&wc);

    HWND hwnd =
        CreateWindow(wc.lpszClassName, L"V-AFK - ANTI-AFK FOR VALORANT", WS_POPUP | WS_VISIBLE, 100,
                     100, 435, 320, NULL, NULL, wc.hInstance, NULL);

    if (appIcon)
    {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)appIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)appIcon);
    }

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();

        UnregisterClass(wc.lpszClassName, wc.hInstance);

        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();

    (void)io;

    SetupStyle();
    SetupFonts();

    ImGui_ImplWin32_Init(hwnd);

    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool done = false;

    while (!done)
    {
        MSG msg;

        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);

            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                done = true;
            }
        }

        ImGui_ImplDX11_NewFrame();

        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        // ==================================================
        // WINDOW
        // ==================================================

        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::SetNextWindowSize(ImVec2(435, 320));

        ImGui::Begin("Main", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize);

        ImDrawList *draw = ImGui::GetWindowDrawList();

        draw->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(435, 320), IM_COL32(14, 15, 18, 255),
                                      IM_COL32(22, 23, 28, 255), IM_COL32(11, 12, 14, 255),
                                      IM_COL32(16, 17, 20, 255));

        draw->AddRect(ImVec2(1, 1), ImVec2(434, 319), IM_COL32(255, 70, 85, 110), 0.0f, 0, 1.0f);

        // ==================================================
        // TOP BAR
        // ==================================================

        draw->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(435, 44), IM_COL32(255, 70, 85, 255),
                                      IM_COL32(204, 52, 64, 255), IM_COL32(150, 38, 47, 255),
                                      IM_COL32(182, 46, 58, 255));

        draw->AddLine(ImVec2(0, 44), ImVec2(435, 44), IM_COL32(255, 155, 165, 120), 1.0f);

        ImGui::PushFont(g_fontTitle);

        ImGui::SetCursorPos(ImVec2(14, 12));

        ImGui::Text("V - A F K");

        ImGui::PopFont();

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(136, 16));

        ImGui::Text("ANTI-AFK FOR VALORANT \\\\ V1.0");

        ImGui::PopFont();

        ImGui::SetCursorPos(ImVec2(395, 11));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.07f, 0.09f, 0.90f));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.12f, 0.16f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.28f, 0.06f, 0.08f, 1.0f));

        if (ImGui::Button("X"))
        {
            PostQuitMessage(0);
        }

        ImGui::PopStyleColor(3);

        // ==================================================
        // STATUS
        // ==================================================

        bool botRunning = g_bot.IsRunning();

        draw->AddRectFilled(ImVec2(16, 56), ImVec2(419, 213), IM_COL32(22, 23, 28, 225), 0.0f);

        draw->AddRect(ImVec2(16, 56), ImVec2(419, 213), IM_COL32(255, 70, 85, 95), 0.0f, 0, 1.0f);

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(24, 66));

        ImGui::SetWindowFontScale(1.0f);

        ImGui::TextColored(ImVec4(0.96f, 0.76f, 0.79f, 1), "PROTOCOL STATUS");

        ImGui::SetWindowFontScale(1.0f);

        ImGui::PopFont();

        ImGui::PushFont(g_fontTitle);

        ImGui::SetCursorPos(ImVec2(24, 95));

        ImGui::SetWindowFontScale(1.45f);

        if (botRunning)
        {
            ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.35f, 1), "ACTIVE");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.75f, 0.8f, 1), "STANDBY");
        }

        ImGui::SetWindowFontScale(1.0f);

        ImGui::PopFont();

        draw->AddLine(ImVec2(24, 149), ImVec2(411, 149), IM_COL32(255, 70, 85, 90), 1.0f);

        // ==================================================
        // SESSION
        // ==================================================

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(24, 164));

        ImGui::TextColored(ImVec4(0.77f, 0.77f, 0.80f, 1), "SESSION");

        ImGui::PopFont();

        float session = g_bot.GetSessionTime();

        int hours = (int)session / 3600;

        int minutes = ((int)session % 3600) / 60;

        int seconds = (int)session % 60;

        ImGui::PushFont(g_fontMono);

        ImGui::SetCursorPos(ImVec2(24, 188));

        ImGui::SetWindowFontScale(1.10f);

        ImGui::Text("%02d:%02d:%02d", hours, minutes, seconds);

        ImGui::SetWindowFontScale(1.0f);

        ImGui::PopFont();

        // ==================================================
        // ACTIONS
        // ==================================================

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(240, 164));

        ImGui::TextColored(ImVec4(0.77f, 0.77f, 0.80f, 1), "ACTIONS");

        ImGui::PopFont();

        ImGui::PushFont(g_fontMono);

        ImGui::SetCursorPos(ImVec2(240, 188));

        ImGui::SetWindowFontScale(1.10f);

        ImGui::Text("%d", g_bot.GetActions());

        ImGui::SetWindowFontScale(1.0f);

        ImGui::PopFont();

        // ==================================================
        // BUTTONS
        // ==================================================

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(22, 236));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 0.27f, 0.34f, 1));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.00f, 0.36f, 0.43f, 1));

        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.82f, 0.20f, 0.27f, 1));

        if (botRunning)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("ACTIVATE", ImVec2(190, 42)))
        {
            g_bot.Start();
        }

        if (botRunning)
        {
            ImGui::EndDisabled();
        }

        ImGui::PopStyleColor(3);

        ImGui::SetCursorPos(ImVec2(225, 236));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.22f, 0.25f, 1));

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.30f, 0.34f, 1));

        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.17f, 0.20f, 1));

        if (!botRunning)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("ABORT", ImVec2(188, 42)))
        {
            g_bot.Stop();
        }

        if (!botRunning)
        {
            ImGui::EndDisabled();
        }

        ImGui::PopStyleColor(3);

        ImGui::PopFont();

        // ==================================================
        // FOOTER
        // ==================================================

        ImGui::PushFont(g_fontUi);

        ImGui::SetCursorPos(ImVec2(18, 292));

        ImGui::TextColored(ImVec4(0.88f, 0.54f, 0.58f, 1), "SESSION GUARD");

        ImGui::PopFont();

        ImGui::PushFont(g_fontMono);

        ImGui::SetCursorPos(ImVec2(290, 292));

        ImGui::TextColored(ImVec4(0.67f, 0.67f, 0.72f, 1), "Ar-mane@github");

        ImGui::PopFont();

        ImGui::End();

        // ==================================================
        // RENDER
        // ==================================================

        ImGui::Render();

        const float clear_color[4] = {0.05f, 0.05f, 0.06f, 1.0f};

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    g_bot.Stop();

    ImGui_ImplDX11_Shutdown();

    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    CleanupDeviceD3D();

    DestroyWindow(hwnd);

    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}