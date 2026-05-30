#pragma once

#include "imgui.h"

#include <cmath>

namespace ui
{
inline ImU32 Pick(bool on, ImU32 off, ImU32 onVal)
{
    return on ? onVal : off;
}

inline ImU32 AlphaPulse(ImU32 color, float speed, int minA, int maxA)
{
    const float wave = 0.5f + 0.5f * std::sin(static_cast<float>(ImGui::GetTime()) * speed);
    const int alpha = minA + static_cast<int>((maxA - minA) * wave);
    return (color & 0x00FFFFFFu) | (static_cast<ImU32>(alpha) << 24);
}

inline void TextAt(ImFont* font, ImVec2 pos, ImVec4 color, const char* text, float scale = 1.f)
{
    if (font)
    {
        ImGui::PushFont(font, font->LegacySize * scale);
    }

    ImGui::SetCursorPos(pos);
    ImGui::TextColored(color, "%s", text);

    if (font)
    {
        ImGui::PopFont();
    }
}

inline void DrawCenteredInRect(ImDrawList* dl, ImFont* font, ImVec2 min, ImVec2 max, ImU32 color, const char* text, float yBias = 0.f)
{
    if (!font)
    {
        return;
    }

    ImGui::PushFont(font, font->LegacySize);
    const ImVec2 ts = ImGui::CalcTextSize(text);
    const ImVec2 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
    dl->AddText(ImVec2(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f + yBias), color, text);
    ImGui::PopFont();
}

inline bool
HitBox(const char* id, ImVec2 pos, ImVec2 size, ImU32 fill, ImU32 fillHot, ImU32 border, ImU32 borderHot, float round, float borderW = 1.5f)
{
    ImGui::SetCursorPos(pos);
    ImGui::PushID(id);
    ImGui::InvisibleButton("##hit", size);

    const bool hot = ImGui::IsItemHovered();
    const bool click = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    const ImVec2 a = ImGui::GetItemRectMin();
    const ImVec2 b = ImGui::GetItemRectMax();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRectFilled(a, b, Pick(hot || ImGui::IsItemActive(), fill, fillHot), round);
    if (border)
    {
        dl->AddRect(a, b, Pick(hot, border, borderHot), round, 0, hot ? 2.f : borderW);
    }

    if (hot)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::PopID();
    return click;
}

} // namespace ui
