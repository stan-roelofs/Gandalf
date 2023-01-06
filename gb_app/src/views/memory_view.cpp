#include "memory_view.h"

#include <imgui.h>
#include <optional>

#include "../text.h"
namespace gui
{
    MemoryView::MemoryView(const bool& debug_enabled):
        debug_enabled_(debug_enabled)
    {
    }

    void MemoryView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin(text::Get(text::ID::WindowMemory), nullptr, ImGuiWindowFlags_NoTitleBar);

        static char address[5];
        address[4] = '\0';
        ImGui::InputText("Address", address, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();

        std::optional<gandalf::word> scroll_target;
        bool should_scroll = false;
        if (ImGui::Button(text::Get(text::ID::WindowMemoryScrollToAddress))) {
            auto address_value = std::strtoul(address, nullptr, 16);
            if (address_value < 0)
                address_value = 0;
            if (address_value > 0xFFFF)
                address_value = 0xFFFF;

            scroll_target = static_cast<gandalf::word>(address_value);
        }

        if (ImGui::BeginTable(text::Get(text::ID::WindowMemoryViewer), 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp)) {
            static float memory_item_height = 0.f;

            if (scroll_target)
                ImGui::SetScrollY((*scroll_target / 16) * memory_item_height);

            const gandalf::Memory& memory = gameboy_->GetMemory();
            ImGuiListClipper clipper;
            clipper.Begin(0x10000 / 16);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    gandalf::word address_start = line_no * 16;

                    ImGui::TableNextColumn();
                    ImGui::Text("%04X", address_start);
                    for (int column = 0; column < 16; column++)
                    {
                        const gandalf::word address = address_start + column;
                        const gandalf::byte value = gameboy_->GetMemory().Read(address, false);
                        ImGui::TableNextColumn();
                        ImGui::Text("%02X", value);
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::Text("%s: %s", text::Get(text::ID::WindowMemoryViewerAddressOwner), gameboy_->GetMemory().GetAddressHandlerName(address).c_str());
                            ImGui::Text("%s: %04X", text::Get(text::ID::WindowMemoryViewerAddress), address);
                            ImGui::Text("%s: %02X", text::Get(text::ID::WindowMemoryViewerAddressValue), value);
                            ImGui::EndTooltip();
                        }
                    }
                }

                if (clipper.ItemsHeight > 0) memory_item_height = clipper.ItemsHeight;
            }
            clipper.End();

            ImGui::EndTable();
        }

        ImGui::End();
    }
}