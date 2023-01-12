#include "io_view.h"

#include <imgui.h>

#include "../text.h"

namespace gui
{
    IOView::IOView(const bool& debug_enabled):
        debug_enabled_(debug_enabled)
    {
    }

    void IOView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin(text::Get(text::ID::WindowIO), nullptr, ImGuiWindowFlags_NoTitleBar);

        // Timer
        const auto& timer = gameboy_->GetTimer();
        ImGui::Text("DIV Internal counter: %04X", timer.GetInternalCounter());
        ImGui::Text("FF04 DIV: %02X", timer.GetDIV());
        ImGui::Text("FF05 TIMA: %02X", timer.GetTIMA());
        ImGui::Text("FF06 TMA: %02X", timer.GetTMA());
        ImGui::Text("FF07 TAC: %02X", timer.GetTAC());

        ImGui::Separator();

        // LCD
        const auto& lcd = gameboy_->GetLCD();
        ImGui::Text("FF40 LCDC: %02X", lcd.GetLCDControl());
        ImGui::Text("FF41 STAT: %02X", lcd.GetLCDStatus());
        ImGui::Text("FF42 SCY: %02X", lcd.GetSCY());
        ImGui::Text("FF43 SCX: %02X", lcd.GetSCX());
        ImGui::Text("FF44 LY: %02X", lcd.GetLY());
        ImGui::Text("FF45 LYC: %02X", lcd.GetLYC());
        //ImGui::Text("FF46 DMA: %02X", );
        ImGui::Text("FF47 BGP: %02X", lcd.GetBGP());
        ImGui::Text("FF48 OBP0: %02X", lcd.GetOBP0());
        ImGui::Text("FF49 OBP1: %02X", lcd.GetOBP1());
        ImGui::Text("FF4A WY: %02X", lcd.GetWY());
        ImGui::Text("FF4B WX: %02X", lcd.GetWX());

        ImGui::End();
    }
}