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
        const auto& cart = gameboy_->GetCartridge();
        if (cart.Loaded())
        {
            const auto& header = cart.GetHeader();
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeTitle), header->GetTitleString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeManufacturer), header->GetManufacturerCodeString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeLicensee), header->GetLicenseeString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeROMSize), header->GetROMSizeString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeRAMSize), header->GetRAMSizeString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeCGB), header->GetCGBFlagString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeSGB), header->GetSGBFlagString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeType), header->GetTypeString().c_str());
            ImGui::Text("%s: %s", text::Get(text::ID::WindowCartridgeDestination), header->GetDestinationString().c_str());
        }
        else
            ImGui::TextUnformatted("No ROM loaded");
        ImGui::End();
    }
}