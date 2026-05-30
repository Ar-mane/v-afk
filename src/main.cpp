#include "app_icon.h"
#include "bot.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ui/main_view.h"
#include "util/path.h"

#include <d3d11.h>
#include <string>
#include <windows.h>
#include <windowsx.h>

#pragma comment(lib, "d3d11.lib")

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static Bot g_bot;

static ImFont* LoadFontFromResource(ImGuiIO& io, int resourceId, float size, ImFontConfig* config)
{
    HRSRC res = FindResourceW(nullptr, MAKEINTRESOURCEW(resourceId), RT_RCDATA);
    if (!res)
    {
        return nullptr;
    }

    HGLOBAL mem = LoadResource(nullptr, res);
    if (!mem)
    {
        return nullptr;
    }

    void* data = LockResource(mem);
    const DWORD nbytes = SizeofResource(nullptr, res);
    if (!data || nbytes == 0)
    {
        return nullptr;
    }

    ImFontConfig cfg = config ? *config : ImFontConfig{};
    cfg.FontDataOwnedByAtlas = false;
    return io.Fonts->AddFontFromMemoryTTF(data, static_cast<int>(nbytes), size, &cfg);
}

static ImFont* LoadFont(ImGuiIO& io, int resourceId, const char* relative, float size, ImFontConfig* config)
{
    ImFont* font = LoadFontFromResource(io, resourceId, size, config);
    if (font)
    {
        return font;
    }

    font = io.Fonts->AddFontFromFileTTF(util::PathNextToExe(relative).c_str(), size, config);
    if (font)
    {
        return font;
    }

    return io.Fonts->AddFontFromFileTTF(relative, size, config);
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;

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

    if (D3D11CreateDeviceAndSwapChain(NULL,
                                      D3D_DRIVER_TYPE_HARDWARE,
                                      NULL,
                                      createDeviceFlags,
                                      featureLevelArray,
                                      1,
                                      D3D11_SDK_VERSION,
                                      &sd,
                                      &g_pSwapChain,
                                      &g_pd3dDevice,
                                      &featureLevel,
                                      &g_pd3dDeviceContext) != S_OK)
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

                if (pt.y >= 0 && pt.y < ui::kTopBarHeight && pt.x < ui::kCloseButtonLeft)
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

static void SetupStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0.f;
    s.FrameRounding = 4.f;
    s.WindowBorderSize = 0.f;
    s.FrameBorderSize = 0.f;
    s.WindowPadding = ImVec2(0, 0);
    s.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.06f, 1.f);
    s.Colors[ImGuiCol_Text] = ImVec4(0.93f, 0.93f, 0.95f, 1.f);
    s.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
}

static UiFonts SetupFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConfig = {};
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;

    UiFonts fonts;
    fonts.ui = LoadFont(io, IDR_FONT_UI, "assets/fonts/Orbitron/Orbitron-Medium.ttf", 13.f, &fontConfig);
    fonts.title = LoadFont(io, IDR_FONT_TITLE, "assets/fonts/Orbitron/Orbitron-Bold.ttf", 17.5f, &fontConfig);
    fonts.mono = LoadFont(io, IDR_FONT_MONO, "assets/fonts/Fira_Code/FiraCode-Medium.ttf", 14.f, &fontConfig);

    if (!fonts.ui)
    {
        fonts.ui = io.Fonts->AddFontDefault();
    }

    if (!fonts.title)
    {
        fonts.title = fonts.ui;
    }

    if (!fonts.mono)
    {
        fonts.mono = fonts.ui;
    }

    io.FontDefault = fonts.ui;
    return fonts;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    HICON appIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInstance, appIcon, LoadCursor(NULL, IDC_ARROW), NULL, NULL, L"V-AFK", appIcon};

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName,
                             L"V-AFK - ANTI-AFK FOR VALORANT",
                             WS_POPUP | WS_VISIBLE,
                             100,
                             100,
                             static_cast<int>(ui::kWindowWidth),
                             static_cast<int>(ui::kWindowHeight),
                             NULL,
                             NULL,
                             wc.hInstance,
                             NULL);

    if (appIcon)
    {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)appIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)appIcon);
    }

    {
        const int w = static_cast<int>(ui::kWindowWidth);
        const int h = static_cast<int>(ui::kWindowHeight);
        const int r = static_cast<int>(ui::kWindowRounding) * 2;
        const HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, r, r);
        SetWindowRgn(hwnd, rgn, TRUE);
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
    SetupStyle();

    const UiFonts fonts = SetupFonts();

    ImGui_ImplWin32_Init(hwnd);

    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    ImGui_ImplDX11_CreateDeviceObjects();

    const UiContext uiContext = {fonts, &g_bot};

    bool done = false;
    bool topMost = false;

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

        const bool running = g_bot.IsRunning();
        if (running != topMost)
        {
            topMost = running;
            SetWindowPos(hwnd, topMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        DrawMainView(uiContext);

        ImGui::Render();

        const float clearColor[4] = {0.05f, 0.05f, 0.06f, 1.0f};

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);

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
