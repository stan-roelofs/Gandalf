#include "cpu_view.h"

#include <imgui.h>

#include "../text.h"

namespace gui
{
    CPUView::CPUView(const bool& debug_enabled, bool& gb_pause, bool& block_audio, bool& step, std::optional<gandalf::word>& breakpoint):
        debug_enabled_(debug_enabled),
        gb_pause_(gb_pause),
        block_audio_(block_audio),
        step_(step),
        breakpoint_(breakpoint)
    {
    }

    void CPUView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin(text::Get(text::ID::kWindowCPU), nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Checkbox(text::Get(text::ID::kPause), &gb_pause_);
        ImGui::Checkbox(text::Get(text::ID::kWindowCPULimitFPS), &block_audio_);
        ImGui::SameLine();

        if (!gb_pause_)
            ImGui::BeginDisabled();

        if (ImGui::Button(text::Get(text::ID::kWindowCPUStep))) {
            step_ = true;
        }

        if (!gb_pause_)
            ImGui::EndDisabled();

        if (ImGui::BeginTable(text::Get(text::ID::kWindowCPURegisters), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            gandalf::Registers registers = gameboy_->GetCPU().GetRegisters();
            ImGui::TableNextColumn();
            ImGui::Text("A : %02X", registers.a());
            ImGui::TableNextColumn();
            ImGui::Text("F : %02X", registers.f());
            ImGui::TableNextColumn();
            ImGui::Text("B : %02X", registers.b());
            ImGui::TableNextColumn();
            ImGui::Text("C : %02X", registers.c());
            ImGui::TableNextColumn();
            ImGui::Text("D : %02X", registers.d());
            ImGui::TableNextColumn();
            ImGui::Text("E : %02X", registers.e());
            ImGui::TableNextColumn();
            ImGui::Text("H : %02X", registers.h());
            ImGui::TableNextColumn();
            ImGui::Text("L : %02X", registers.l());

            ImGui::TableNextColumn();
            ImGui::Text("SP : %04X", registers.stack_pointer);
            ImGui::TableNextColumn();
            ImGui::Text("PC : %04X", registers.program_counter);

            ImGui::TableNextColumn();

            ImGui::EndTable();
        }

        ImGui::BeginDisabled();
        ImGui::Checkbox("IME", &gameboy_->GetCPU().GetRegisters().interrupt_master_enable);
        ImGui::EndDisabled();

        ImGui::Separator();

        gandalf::Bus& bus = gameboy_->GetBus();
        gandalf::Registers& registers = gameboy_->GetCPU().GetRegisters();
        if (ImGui::BeginTable("Debugger", 3, ImGuiTableFlags_ScrollY)) {
            static gandalf::word last_pc = registers.program_counter;
            static float debugger_item_height = 0.f;
            if (last_pc != registers.program_counter && debugger_item_height > 0) {
                ImGui::SetScrollY(registers.program_counter * debugger_item_height);
                last_pc = registers.program_counter;
            }

            ImGuiListClipper clipper;
            clipper.Begin(0x10000);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    // TODO: decode instructions, show name with operand values, group them (e.g. LD_RR_NN should combine into one line)
                    ImGui::TableNextRow();

                    if (line_no == registers.program_counter)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.5f)));

                    ImGui::TableSetColumnIndex(0);

                    bool dummy = false;
                    std::string label = "##b" + std::to_string(line_no);
                    if (ImGui::Selectable(label.c_str(), breakpoint_ && *breakpoint_ == line_no)) {
                        if (breakpoint_ && *breakpoint_ == line_no)
                            breakpoint_ = std::nullopt;
                        else
                            breakpoint_ = line_no;
                    }

                    ImGui::TableSetColumnIndex(1);

                    ImGui::Text("%04X", line_no);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%02X", bus.DebugRead(line_no));
                }

                if (clipper.ItemsHeight > 0) debugger_item_height = clipper.ItemsHeight;
            }
            clipper.End();

            ImGui::EndTable();
        }

        ImGui::End();
    }
}