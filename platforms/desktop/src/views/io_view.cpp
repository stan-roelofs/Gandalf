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

        ImGui::Text("DIV: %02X", gameboy_->GetTimer().GetDIV());
        ImGui::Text("DIV Internal counter: %04X", gameboy_->GetTimer().GetInternalCounter());

        ImGui::End();
    }
}