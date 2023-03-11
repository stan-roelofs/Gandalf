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

        ImGui::Begin(text::Get(text::ID::WindowCPU), nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::Checkbox(text::Get(text::ID::Pause), &gb_pause_);
        ImGui::Checkbox(text::Get(text::ID::WindowCPULimitFPS), &block_audio_);
        ImGui::SameLine();

        if (!gb_pause_)
            ImGui::BeginDisabled();

        if (ImGui::Button(text::Get(text::ID::WindowCPUStep))) {
            step_ = true;
        }

        if (!gb_pause_)
            ImGui::EndDisabled();

        const auto& cpu = gameboy_->GetCPU();
        const gandalf::Registers& registers = cpu.GetRegisters();

        if (ImGui::BeginTable("Registers", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
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

#define IMGUI_DISABLED_CHECKBOX(label, value) \
            ImGui::BeginDisabled(); \
            ImGui::Checkbox(label, value); \
            ImGui::EndDisabled();

        static bool ime = false;
        ime = cpu.GetRegisters().interrupt_master_enable;
        IMGUI_DISABLED_CHECKBOX("IME", &ime);

        if (ImGui::BeginTable("Interrupts", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::Text("IE : %02X", cpu.GetRegisters().interrupt_enable);
            ImGui::TableNextColumn();
            ImGui::Text("IF : %02X", cpu.GetRegisters().interrupt_flags);
            ImGui::TableNextColumn();

            static bool vblank, lcd, timer, serial, joypad;
            static bool vblank_if, lcd_if, timer_if, serial_if, joypad_if;
            vblank = cpu.GetRegisters().interrupt_enable & gandalf::VBlankInterruptMask;
            lcd = cpu.GetRegisters().interrupt_enable & gandalf::LCDInterruptMask;
            timer = cpu.GetRegisters().interrupt_enable & gandalf::TimerInterruptMask;
            serial = cpu.GetRegisters().interrupt_enable & gandalf::SerialInterruptMask;
            joypad = cpu.GetRegisters().interrupt_enable & gandalf::JoypadInterruptMask;
            vblank_if = cpu.GetRegisters().interrupt_flags & gandalf::VBlankInterruptMask;
            lcd_if = cpu.GetRegisters().interrupt_flags & gandalf::LCDInterruptMask;
            timer_if = cpu.GetRegisters().interrupt_flags & gandalf::TimerInterruptMask;
            serial_if = cpu.GetRegisters().interrupt_flags & gandalf::SerialInterruptMask;
            joypad_if = cpu.GetRegisters().interrupt_flags & gandalf::JoypadInterruptMask;

            ImGui::TextUnformatted("V-Blank");
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##vblank", &vblank);
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##vblank_if", &vblank_if);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("LCD");
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##lcd", &lcd);
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##lcd_if", &lcd_if);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Timer");
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##timer", &timer);
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##timer_if", &timer_if);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Serial");
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##serial", &serial);
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##serial_if", &serial_if);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Joypad");
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##joypad", &joypad);
            ImGui::TableNextColumn();
            IMGUI_DISABLED_CHECKBOX("##joypad_if", &joypad_if);
            ImGui::TableNextColumn();

            ImGui::EndTable();
        }

        ImGui::Separator();

        static char address[5];
        address[4] = '\0';
        ImGui::InputText("Address", address, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();

        std::optional<gandalf::word> scroll_target;
        bool should_scroll = false;
        if (ImGui::Button(text::Get(text::ID::WindowCPUJump))) {
            auto address_value = std::strtoul(address, nullptr, 16);
            if (address_value < 0)
                address_value = 0;
            if (address_value > 0xFFFF)
                address_value = 0xFFFF;

            scroll_target = static_cast<gandalf::word>(address_value);
        }

        const gandalf::Memory& memory = gameboy_->GetMemory();
        if (ImGui::BeginTable("Debugger", 3, ImGuiTableFlags_ScrollY)) {
            static gandalf::word last_pc = registers.program_counter;
            if (last_pc != registers.program_counter) {
                ImGui::SetScrollY(registers.program_counter * ImGui::GetTextLineHeightWithSpacing());
                last_pc = registers.program_counter;
            }

            static float memory_item_height = 0.f;
            if (scroll_target)
                ImGui::SetScrollY(*scroll_target * memory_item_height);

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
                    snprintf(address, 5, "%04X", current_address);
                    if (ImGui::Selectable(address, breakpoint_ && *breakpoint_ == current_address, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns)) {
                        if (breakpoint_ && *breakpoint_ == current_address)
                            breakpoint_ = std::nullopt;
                        else
                            breakpoint_ = current_address;
                    }
                    ImGui::TableNextColumn();

                    auto opcode = memory.Read(current_address++, false);
                    const debugger::Instruction& instruction = debugger::DecodeInstruction(opcode);

                    gandalf::byte op1, op2;
                    if (instruction.length > 0) op1 = memory.Read(current_address++, false);
                    if (instruction.length > 1) op2 = memory.Read(current_address++, false);

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

                if (clipper.ItemsHeight > 0)
                    memory_item_height = clipper.ItemsHeight;
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
}