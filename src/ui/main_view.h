#pragma once

#include "imgui.h"

class Bot;

namespace ui
{
constexpr float kWindowWidth = 440.f;
constexpr float kWindowHeight = 320.f;
constexpr float kMargin = 20.f;
constexpr float kTopBarHeight = 48.f;
constexpr float kCloseButtonLeft = kWindowWidth - kMargin - 30.f;
constexpr float kWindowRounding = 6.f;

} // namespace ui

struct UiFonts
{
    ImFont* title = nullptr;
    ImFont* ui = nullptr;
    ImFont* mono = nullptr;
};

struct UiContext
{
    UiFonts fonts;
    Bot* bot = nullptr;
};

void DrawMainView(const UiContext& ctx);
