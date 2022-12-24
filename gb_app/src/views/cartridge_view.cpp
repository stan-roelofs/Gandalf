#include "cartridge_view.h"

#include <imgui.h>

#include "../text.h"

namespace gui
{
    CartridgeView::CartridgeView(const bool& debug_enabled) :
        debug_enabled_(debug_enabled)
    {
    }

    void CartridgeView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin("Cartridge");
        if (const auto& cartridge_ptr = gameboy_->GetCartridge())
        {
            if (cartridge_ptr->Loaded()) {
                const auto& header = cartridge_ptr->GetHeader();
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeTitle), header->GetTitleString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeManufacturer), header->GetManufacturerCodeString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeLicensee), header->GetLicenseeString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeROMSize), header->GetROMSizeString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeRAMSize), header->GetRAMSizeString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeCGB), header->GetCGBFlagString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeSGB), header->GetSGBFlagString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeType), header->GetTypeString().c_str());
                ImGui::Text("%s: %s", text::Get(text::ID::kWindowCartridgeDestination), header->GetDestinationString().c_str());
            }
        }
        else
            ImGui::TextUnformatted("No ROM loaded");
        ImGui::End();
    }
}