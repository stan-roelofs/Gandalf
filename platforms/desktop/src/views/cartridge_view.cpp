#include "cartridge_view.h"

#include <imgui.h>

namespace gui
{
    CartridgeView::CartridgeView(const bool& debug_enabled):
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
                ImGui::Text("ROM loaded");
                ImGui::Text("Title: %s", header->GetTitleString().c_str());
                ImGui::Text("Manufacturer code: %s", header->GetManufacturerCodeString().c_str());
                ImGui::Text("Licensee: %s", header->GetLicenseeString().c_str());
                ImGui::Text("ROM Size: %s", header->GetROMSizeString().c_str());
                ImGui::Text("RAM Size: %s", header->GetRAMSizeString().c_str());
                ImGui::Text("CGB flag: %s", header->GetCGBFlagString().c_str());
                ImGui::Text("SGB flag: %s", header->GetSGBFlagString().c_str());
                ImGui::Text("Cartridge type: %s", header->GetTypeString().c_str());
                ImGui::Text("Destination: %s", header->GetDestinationString().c_str());
            }
        }
        else
            ImGui::TextUnformatted("No ROM loaded");
        ImGui::End();
    }
}