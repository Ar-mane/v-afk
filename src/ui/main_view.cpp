#include "ui/main_view.h"

#include "bot.h"
#include "imgui.h"
#include "ui/imgui_util.h"
#include "util/color.h"

#include <cmath>
#include <cstdio>
#include <windows.h>

namespace

{
constexpr float kPad = 16.f;

constexpr float kRound = ui::kWindowRounding;

constexpr ImU32 C_BG0 = util::Hex("#0E0F12");

constexpr ImU32 C_BG1 = util::Hex("#0A0B0E");

constexpr ImU32 C_PANEL = util::Hex("#181A20");

constexpr ImU32 C_CARD = util::Hex("#1E2128");

constexpr ImU32 C_LINE = util::Hex("#FFFFFF", 18);

constexpr ImU32 C_STROKE = util::Hex("#FFFFFF", 22);

constexpr ImU32 C_ACCENT = util::Hex("#FF4655", 200);

constexpr ImU32 C_FRAME = util::Hex("#FAFCFF", 180);

constexpr ImVec4 T_MAIN = util::Hex4("#F0F0F5");

constexpr ImVec4 T_MUTED = util::Hex4("#8C919E");

constexpr ImVec4 T_DIM = util::Hex4("#6B707A");

constexpr ImVec4 T_ACCENT = util::Hex4("#F58C94");

constexpr ImVec4 T_RUN = util::Hex4("#59E08C");

constexpr ImVec4 T_IDLE = util::Hex4("#B8BEC7");

struct L

{
    static constexpr float panelTop = ui::kTopBarHeight + 12.f;

    static constexpr float panelBot = panelTop + 210.f;

    static constexpr float statsTop = panelBot - kPad - 52.f;

    static constexpr float divY = statsTop - 10.f;

    static constexpr float innerL = ui::kMargin + kPad;

    static constexpr float innerR = ui::kWindowWidth - ui::kMargin - kPad;

    static constexpr float footY = panelBot + 22.f;
};

} // namespace

void DrawMainView(const UiContext& ctx)

{
    const bool run = ctx.bot->IsRunning();

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::SetNextWindowSize(ImVec2(ui::kWindowWidth, ui::kWindowHeight));

    ImGui::Begin("Main",
                 nullptr,

                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    const ImVec2 panelMin(ui::kMargin, L::panelTop);

    const ImVec2 panelMax(ui::kWindowWidth - ui::kMargin, L::panelBot);

    dl->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(ui::kWindowWidth, ui::kWindowHeight), C_BG0, C_BG0, C_BG1, C_BG1);

    dl->AddRectFilled(ImVec2(0, 0),
                      ImVec2(ui::kWindowWidth, ui::kTopBarHeight),

                      util::Hex("#121318"));

    dl->AddRectFilled(ImVec2(0, ui::kTopBarHeight - 2),
                      ImVec2(ui::kWindowWidth, ui::kTopBarHeight),

                      C_ACCENT);

    dl->AddLine(ImVec2(0, ui::kTopBarHeight), ImVec2(ui::kWindowWidth, ui::kTopBarHeight), C_LINE);

    ui::TextAt(ctx.fonts.title, ImVec2(ui::kMargin, 14), T_MAIN, "V-AFK");

    ui::TextAt(ctx.fonts.ui, ImVec2(ui::kMargin + 68, 18), T_MUTED, "for Valorant");

    const ImVec2 closePos(ui::kCloseButtonLeft, (ui::kTopBarHeight - 30.f) * 0.5f);

    const bool close = ui::HitBox("close",
                                  closePos,
                                  ImVec2(30, 30),
                                  util::Hex("#2A2D36", 200),

                                  util::Hex("#DC303C"),
                                  util::Hex("#FFFFFF", 28),

                                  util::Hex("#FF7884"),
                                  4.f);

    ui::DrawCenteredInRect(dl,
                           ctx.fonts.title,
                           ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(),

                           util::Hex("#F5F5F8"),
                           "X",
                           0.f);

    dl->AddRectFilled(panelMin, panelMax, C_PANEL, kRound);

    dl->AddRect(panelMin, panelMax, C_STROKE, kRound, 0, 1.f);

    const ImVec2 togglePos(panelMin.x + kPad, panelMin.y + kPad);

    const ImVec2 toggleSize(panelMax.x - panelMin.x - kPad * 2, L::divY - 12.f - togglePos.y);

    ImU32 tBorder = run ? util::Hex("#37D278", 210) : util::Hex("#FF4655", 125);

    if (run)

    {
        tBorder = ui::AlphaPulse(util::Hex("#37D278"), 3.2f, 120, 255);
    }

    const ImU32 tFill = run ? util::Hex("#1E3428") : util::Hex("#22242C");

    const ImU32 tFillH = run ? util::Hex("#264232") : util::Hex("#2C2F38");

    const ImU32 tBorderH = run ? util::Hex("#64F0A0") : util::Hex("#FF6470", 200);

    const bool toggle =

        ui::HitBox("toggle", togglePos, toggleSize, tFill, tFillH, tBorder, tBorderH, kRound);

    const ImVec2 tr = ImGui::GetItemRectMin();

    const float dotX = tr.x + 22;

    const float dotY = tr.y + 30;

    const ImU32 dot = run ? util::Hex("#37D278") : util::Hex("#787C84");

    dl->AddCircleFilled(ImVec2(dotX, dotY), 5.f, dot, 16);

    if (run)

    {
        const float glow = 8.f + 2.f * std::sin(static_cast<float>(ImGui::GetTime()) * 3.2f);

        dl->AddCircleFilled(ImVec2(dotX, dotY), glow, util::Hex("#37D278", 40), 16);
    }
    else
    {
        dl->AddCircle(ImVec2(dotX, dotY), 7.5f, util::Hex("#FF4655", 50), 16, 1.f);
    }

    ui::TextAt(ctx.fonts.ui, ImVec2(tr.x + 38, tr.y + 14), T_MUTED, "Status");

    ui::TextAt(ctx.fonts.title,
               ImVec2(tr.x + 38, tr.y + 34),
               run ? T_RUN : T_IDLE,

               run ? "Running" : "Idle",
               1.1f);

    ui::TextAt(ctx.fonts.ui,
               ImVec2(tr.x + 38, tr.y + 62),
               T_MUTED,

               run ? "Tap to disarm" : "Tap to arm");

    if (!run)

    {
        ui::TextAt(ctx.fonts.ui, ImVec2(tr.x + 38, tr.y + 82), T_DIM, "Local input only");
    }

    dl->AddLine(ImVec2(L::innerL, L::divY), ImVec2(L::innerR, L::divY), C_LINE);

    const ImVec2 statsMin(L::innerL, L::statsTop);

    const ImVec2 statsMax(L::innerR, L::statsTop + 52.f);

    dl->AddRectFilled(statsMin, statsMax, C_CARD, kRound);

    dl->AddRect(statsMin, statsMax, C_STROKE, kRound, 0, 1.f);

    const float midX = (statsMin.x + statsMax.x) * 0.5f;

    dl->AddLine(ImVec2(midX, statsMin.y + kPad), ImVec2(midX, statsMax.y - kPad), C_LINE);

    const float session = ctx.bot->GetSessionTime();

    const int h = static_cast<int>(session) / 3600;

    const int m = (static_cast<int>(session) % 3600) / 60;

    const int s = static_cast<int>(session) % 60;

    char sessionBuf[16];
    char scenarioBuf[24];

    snprintf(sessionBuf, sizeof(sessionBuf), "%02d:%02d:%02d", h, m, s);

    ctx.bot->GetScenarioPreview(scenarioBuf, sizeof(scenarioBuf));
    if (scenarioBuf[0] == '\0')
    {
        snprintf(scenarioBuf, sizeof(scenarioBuf), "-");
    }

    const float ty = L::statsTop + kPad;

    ui::TextAt(ctx.fonts.ui, ImVec2(statsMin.x + kPad, ty), T_MUTED, "Session");

    ui::TextAt(ctx.fonts.ui, ImVec2(midX + kPad, ty), T_MUTED, "Scenario");

    ui::TextAt(ctx.fonts.mono, ImVec2(statsMin.x + kPad, ty + 20), T_MAIN, sessionBuf);

    ui::TextAt(ctx.fonts.mono, ImVec2(midX + kPad, ty + 20), T_MAIN, scenarioBuf);

    dl->AddLine(ImVec2(ui::kMargin, L::footY),
                ImVec2(ui::kWindowWidth - ui::kMargin, L::footY),

                C_LINE);

    ui::TextAt(ctx.fonts.ui, ImVec2(ui::kMargin, L::footY + 6), T_ACCENT, "Session guard . v1.0");

    const char* credit = "Ar-mane@github";

    ImGui::PushFont(ctx.fonts.mono, ctx.fonts.mono->LegacySize);

    const ImVec2 creditSize = ImGui::CalcTextSize(credit);

    ImGui::PopFont();

    ui::TextAt(ctx.fonts.mono,
               ImVec2(ui::kWindowWidth - ui::kMargin - creditSize.x, L::footY + 6),

               T_MUTED,
               credit);

    if (close)

    {
        PostQuitMessage(0);
    }

    if (toggle)

    {
        run ? ctx.bot->Stop() : ctx.bot->Start();
    }

    dl->AddRect(ImVec2(0.5f, 0.5f),
                ImVec2(ui::kWindowWidth - 0.5f, ui::kWindowHeight - 0.5f),

                C_FRAME,
                kRound,
                0,
                1.f);

    ImGui::End();
}
