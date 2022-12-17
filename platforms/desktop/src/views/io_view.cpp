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

        ImGui::Begin(text::Get(text::ID::kWindowIO), nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("DIV Internal counter: %04X", gameboy_->GetTimer().GetInternalCounter());
        ImGui::Text("DIV: %02X", gameboy_->GetTimer().GetDIV());
        ImGui::Text("TIMA: %02X", gameboy_->GetTimer().GetTIMA());
        ImGui::Text("TMA: %02X", gameboy_->GetTimer().GetTMA());
        ImGui::Text("TAC: %02X", gameboy_->GetTimer().GetTAC());

        ImGui::End();
    }
}