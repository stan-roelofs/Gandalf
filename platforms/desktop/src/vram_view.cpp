#include "vram_view.h"

#include <imgui.h>

namespace
{
    constexpr int kTileSize = 8;
    constexpr int kScale = 2;
    constexpr int kTileSizePx = kTileSize * kScale;
    constexpr int kScaleTooltip = 8;
}

namespace gui
{
    VRAMView::VRAMView(const bool& debug_enabled, SDL_Renderer& renderer) : debug_enabled_(debug_enabled), visible_(false)
    {
        texture_ = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, gandalf::kTotalScreenWidth, gandalf::kTotalScreenHeight);
        if (!texture_)
            throw std::runtime_error("Failed to create texture!");

        for (auto& row : tile_data_)
            row.fill(TileData());
    }

    VRAMView::~VRAMView()
    {
        SDL_DestroyTexture(texture_);
    }

    void VRAMView::SetGameboy(std::shared_ptr<gandalf::Gameboy> gameboy)
    {
        GUIElement::SetGameboy(gameboy);
        gameboy_->GetPPU().AddVBlankListener(this);
    }

    void VRAMView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin("PPU");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
        if (ImGui::BeginTabBar("PPUTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Background map"))
            {
                visible_ = true;
                SDL_UpdateTexture(texture_, nullptr, vram_buffer_.data(), gandalf::kTotalScreenWidth * sizeof(gandalf::LCD::BGR555));
                ImVec2 pos = ImGui::GetCursorScreenPos();

                ImGui::Image(texture_, ImVec2(gandalf::kTotalScreenWidth * kScale, gandalf::kTotalScreenHeight * kScale));

                if (ImGui::IsItemHovered())
                {
                    ImGuiIO& io = ImGui::GetIO();
                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

                    ImGui::BeginTooltip();

                    const int tile_x = (io.MousePos.x - pos.x) / kTileSizePx;
                    const int tile_y = (io.MousePos.y - pos.y) / kTileSizePx;

                    const int region_x = tile_x * kTileSize;
                    const int region_y = tile_y * kTileSize;
                    
                    // TODO draw grid

                    constexpr float kTextureSize = gandalf::kTotalScreenWidth;

                    ImVec2 uv0 = ImVec2(region_x / kTextureSize, region_y / kTextureSize);
                    ImVec2 uv1 = ImVec2((region_x + kTileSize) / kTextureSize, (region_y + kTileSize) / kTextureSize);
                    ImGui::Image(texture_, ImVec2(kTileSize * kScaleTooltip, kTileSize * kScaleTooltip), uv0, uv1, tint_col, border_col);

                    ImGui::Separator();
                    assert(tile_x < 32 && tile_y < 32);
                    auto& tile_data = tile_data_[tile_y][tile_x];
                    ImGui::BeginTable("Tile data", 2);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("X"); 
                    ImGui::TableNextColumn();    
                    ImGui::Text("%d", tile_x);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Y");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", tile_y);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Number");
                    ImGui::TableNextColumn();
                    ImGui::Text("%2.2X", tile_data.tile_number);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Tile map");
                    ImGui::TableNextColumn();
                    ImGui::Text("%4.4X", tile_data.map_address);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Tile address");
                    ImGui::TableNextColumn();
                    ImGui::Text("%4.4X", tile_data.tile_address);
                    ImGui::EndTable();

                    ImGui::Separator();
                    ImGui::Text("Attributes: %X", tile_data.attributes);
                    ImGui::Checkbox("Flip x", &tile_data.x_flip); ImGui::SameLine(); ImGui::Checkbox("Flip y", &tile_data.y_flip);
                    ImGui::Text("Palette: %d", tile_data.palette);
                    ImGui::Text("VRAM Bank: %d", tile_data.vram_bank);
                    ImGui::Checkbox("Priority", &tile_data.priority);

                    ImGui::EndTooltip();
                }

                ImGui::EndTabItem();
            }
            else
                visible_ = false;

            if (ImGui::BeginTabItem("Tiles"))
            {

            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void VRAMView::OnVBlank()
    {
        if (!visible_ || !gameboy_)
            return;

        const auto& lcd = gameboy_->GetLCD();
        const auto& ppu = gameboy_->GetPPU();
        const gandalf::byte lcdc = gameboy_->GetLCD().GetLCDControl();
        const bool tile_map_select = (lcdc & 0x08) != 0;

        const gandalf::word map = tile_map_select ? 0x1C00 : 0x1800;
        const gandalf::word tile_data_area = (lcdc & 0x10) == 0 ? 0x1000 : 0;


        for (int tile_y = 0; tile_y < 32; ++tile_y)
        {
            for (int tile_x = 0; tile_x < 32; ++tile_x)
            {
                TileData& tile_data = tile_data_.at(tile_y).at(tile_x);
                tile_data.tile_number = ppu.DebugReadVRam(0, map + tile_y * 32 + tile_x);
                tile_data.attributes = gameboy_->GetMode() == gandalf::GameboyMode::CGB ? ppu.DebugReadVRam(1, map + tile_y * 32 + tile_x) : 0;
                tile_data.y_flip = (tile_data.attributes & 0b1000000) != 0;
                tile_data.x_flip = (tile_data.attributes & 0b100000) != 0;
                tile_data.vram_bank = (tile_data.attributes & 0b1000) >> 3;
                tile_data.palette = tile_data.attributes & 0b111;
                tile_data.map_address = map + 0x8000;
                tile_data.priority = (tile_data.attributes & 0x80) != 0;

                const int tile_offset = (tile_data_area == 0 ? tile_data.tile_number : (gandalf::signed_byte)(tile_data.tile_number));
                tile_data.tile_address = 0x8000 + (tile_data_area + tile_offset * 16);

                for (int line = 0; line < 8; ++line)
                {
                    const gandalf::byte tile_data_low = ppu.DebugReadVRam(tile_data.vram_bank, tile_data_area + tile_offset * 16 + (tile_data.y_flip ? (7 - line) : line) * 2);
                    const gandalf::byte tile_data_high = ppu.DebugReadVRam(tile_data.vram_bank, tile_data_area + tile_offset * 16 + (tile_data.y_flip ? (7 - line) : line) * 2 + 1);

                    for (int x = 0; x < 8; ++x)
                    {
                        const gandalf::byte color_bit_0 = tile_data.x_flip ? !!((tile_data_low) & (1 << x)) : !!(tile_data_low & (1 << (7 - x)));
                        const gandalf::byte color_bit_1 = tile_data.x_flip ? !!((tile_data_high) & (1 << x)) : !!(tile_data_high & (1 << (7 - x)));
                        const gandalf::byte color = color_bit_0 | (color_bit_1 << 1);

                        const gandalf::LCD::BGR555 bgr_color = lcd.GetBackgroundColor(color, tile_data.palette);
                        vram_buffer_[((tile_y * 8 + line) * gandalf::kTotalScreenWidth) + tile_x * 8 + x] = bgr_color;
                    }
                }

                tile_data_.at(tile_y).at(tile_x) = tile_data;
            }
        }

        const gandalf::byte left = lcd.GetSCX();
        const gandalf::byte top = lcd.GetSCY();
        const gandalf::byte right = left + gandalf::kScreenWidth;
        const gandalf::byte bottom = lcd.GetSCY() + gandalf::kScreenHeight;
        const gandalf::LCD::BGR555 color = 0b11111;
        for (int x = 0; x < gandalf::kScreenWidth; ++x)
        {
            vram_buffer_[gandalf::kTotalScreenWidth * top + (gandalf::byte)(left + x)] = color;
            vram_buffer_[gandalf::kTotalScreenWidth * bottom + (gandalf::byte)(left + x)] = color;
        }
        for (int y = 0; y < gandalf::kScreenHeight; ++y)
        {
            vram_buffer_[gandalf::kTotalScreenWidth * (gandalf::byte)(top + y) + left] = color;
            vram_buffer_[gandalf::kTotalScreenWidth * (gandalf::byte)(top + y) + right] = color;
        }
    }
}