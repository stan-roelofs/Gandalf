
#include "apu_view.h"

#include <imgui.h>

namespace gui
{
    APUView::APUView(const bool& debug_enabled):
        debug_enabled_(debug_enabled)
    {
    }

    void APUView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin("APU");
        static bool channel_enabled[4] = { true, true, true, true };
        if (ImGui::Checkbox("Square wave 1", &channel_enabled[0]))
            gameboy_->GetAPU().MuteChannel(0, !channel_enabled[0]);
        if (ImGui::Checkbox("Square wave 2", &channel_enabled[1]))
            gameboy_->GetAPU().MuteChannel(1, !channel_enabled[1]);
        if (ImGui::Checkbox("Wave", &channel_enabled[2]))
            gameboy_->GetAPU().MuteChannel(2, !channel_enabled[2]);
        if (ImGui::Checkbox("Noise", &channel_enabled[3]))
            gameboy_->GetAPU().MuteChannel(3, !channel_enabled[3]);
        ImGui::End();
    }
}