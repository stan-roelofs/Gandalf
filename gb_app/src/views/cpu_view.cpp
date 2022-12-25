#include "cpu_view.h"

#include <imgui.h>

#include "debugger.h"
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

        gandalf::Memory& memory = gameboy_->GetMemory();
        gandalf::Registers& registers = gameboy_->GetCPU().GetRegisters();
        if (ImGui::BeginTable("Debugger", 3, ImGuiTableFlags_ScrollY)) {
            static gandalf::word last_pc = registers.program_counter;
            if (last_pc != registers.program_counter) {
                ImGui::SetScrollY(registers.program_counter * ImGui::GetTextLineHeightWithSpacing());
                last_pc = registers.program_counter;
            }

            ImGuiListClipper clipper(0x10000, ImGui::GetTextLineHeightWithSpacing());
            while (clipper.Step())
            {
                gandalf::word current_address = clipper.DisplayStart;
                for (int line_no = clipper.DisplayStart; line_no <= clipper.DisplayEnd; ++line_no)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);

                    if (current_address == registers.program_counter)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 0.0f, 0.5f)));

                    char address[5];
                    address[4] = '\0';
                    snprintf(address, 5, "%04X##b", current_address);
                    if (ImGui::Selectable(address, breakpoint_ && *breakpoint_ == current_address, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns)) {
                        if (breakpoint_ && *breakpoint_ == current_address)
                            breakpoint_ = std::nullopt;
                        else
                            breakpoint_ = current_address;
                    }
                    ImGui::TableNextColumn();

                    auto opcode = memory.DebugRead(current_address++);
                    const debugger::Instruction& instruction = debugger::DecodeInstruction(opcode);

                    gandalf::byte op1, op2;
                    if (instruction.length > 0) op1 = memory.DebugRead(current_address++);
                    if (instruction.length > 1) op2 = memory.DebugRead(current_address++);

                    switch (instruction.length) {
                    case 0:
                        ImGui::Text("%02X", opcode);
                        break;
                    case 1:
                        ImGui::Text("%02X %02X", opcode, op1);
                        break;
                    case 2:
                        ImGui::Text("%02X %04X", opcode, op1 | (op2 << 8));
                        break;
                    }

                    ImGui::TableNextColumn();
                    switch (instruction.length) {
                    case 0:
                        ImGui::TextUnformatted(instruction.name);
                        break;
                    case 1:
                        ImGui::Text(instruction.name, op1);
                        break;
                    case 2:
                        ImGui::Text(instruction.name, op1 | (op2 << 8));
                        break;
                    }
                }
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
}