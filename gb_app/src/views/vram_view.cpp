#include "vram_view.h"

#include <imgui.h>

#include "../text.h"
namespace
{
    constexpr int kTileSize = 8;
    constexpr int kScale = 2;
    constexpr int kTileSizePx = kTileSize * kScale;
    constexpr int kScaleTooltip = 8;
}

namespace gui
{
    VRAMView::VRAMView(const bool& debug_enabled) : debug_enabled_(debug_enabled), visible_(false)
    {
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (auto& row : tile_data_)
            row.fill(TileData());

        vram_buffer_.fill(0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, gandalf::kTotalScreenWidth, gandalf::kTotalScreenHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram_buffer_.data());
    }

    VRAMView::~VRAMView()
    {
        glDeleteTextures(1, &texture_);
    }

    void VRAMView::OnGameboyChanged()
    {
        gameboy_->GetPPU().AddVBlankListener(this);
    }

    void VRAMView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin(text::Get(text::ID::kWindowPPU));

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
        if (ImGui::BeginTabBar("PPUTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem(text::Get(text::ID::kWindowPPUBackgroundMap)))
            {
                glBindTexture(GL_TEXTURE_2D, texture_);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gandalf::kTotalScreenWidth, gandalf::kTotalScreenHeight, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram_buffer_.data());

                visible_ = true;
                ImVec2 pos = ImGui::GetCursorScreenPos();

                ImGui::Image((void*)(intptr_t)texture_, ImVec2(gandalf::kTotalScreenWidth * kScale, gandalf::kTotalScreenHeight * kScale));

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
                    ImGui::Image((void*)(intptr_t)texture_, ImVec2(kTileSize * kScaleTooltip, kTileSize * kScaleTooltip), uv0, uv1, tint_col, border_col);

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
                    ImGui::TextUnformatted(text::Get(text::ID::kWindowPPUTileNumber));
                    ImGui::TableNextColumn();
                    ImGui::Text("%2.2X", tile_data.tile_number);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(text::Get(text::ID::kWindowPPUTileMap));
                    ImGui::TableNextColumn();
                    ImGui::Text("%4.4X", tile_data.map_address);

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(text::Get(text::ID::kWindowPPUTileAddress));
                    ImGui::TableNextColumn();
                    ImGui::Text("%4.4X", tile_data.tile_address);
                    ImGui::EndTable();

                    ImGui::Separator();
                    ImGui::Text("%s: %X", text::Get(text::ID::kWindowPPUTileAttributes), tile_data.attributes);
                    ImGui::Checkbox(text::Get(text::ID::kWindowPPUTileFlipX), &tile_data.x_flip); ImGui::SameLine(); ImGui::Checkbox(text::Get(text::ID::kWindowPPUTileFlipY), &tile_data.y_flip);
                    ImGui::Text("%s: %d", text::Get(text::ID::kWindowPPUTilePalette), tile_data.palette);
                    ImGui::Text("%s: %d", text::Get(text::ID::kWindowPPUTileVRAMBank), tile_data.vram_bank);
                    ImGui::Checkbox(text::Get(text::ID::kWindowPPUTilePriority), &tile_data.priority);

                    ImGui::EndTooltip();
                }

                ImGui::EndTabItem();
            }
            else
                visible_ = false;

            if (ImGui::BeginTabItem(text::Get(text::ID::kWindowPPUTiles)))
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

                        const gandalf::LCD::ABGR1555 abgr_color = lcd.GetBackgroundColor(color, tile_data.palette) | 0x8000;
                        vram_buffer_[((tile_y * 8 + line) * gandalf::kTotalScreenWidth) + tile_x * 8 + x] = abgr_color;
                    }
                }

                tile_data_.at(tile_y).at(tile_x) = tile_data;
            }
        }

        // Draw a rectangle to show the currently visible area
        const gandalf::byte left = lcd.GetSCX();
        const gandalf::byte top = lcd.GetSCY();
        const gandalf::byte right = left + gandalf::kScreenWidth;
        const gandalf::byte bottom = lcd.GetSCY() + gandalf::kScreenHeight;
        const gandalf::LCD::ABGR1555 color = 0b11111 | 0x8000;
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