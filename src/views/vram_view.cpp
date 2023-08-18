#include "vram_view.h"

#include <imgui.h>

#include "../text.h"
namespace
{
    constexpr int kTileSize = 8;
    constexpr int kScale = 2;
    constexpr int kTileSizePx = kTileSize * kScale;
    constexpr int kScaleTooltip = 8;
    constexpr int kSpriteScale = 4;
}

namespace gui
{
    VRAMView::VRAMView(const bool& debug_enabled) : debug_enabled_(debug_enabled), current_tab_(Tab::Background)
    {
        glGenTextures(1, &background_texture_);
        glBindTexture(GL_TEXTURE_2D, background_texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        vram_buffer_.fill(0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, gandalf::TotalScreenWidth, gandalf::TotalScreenHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram_buffer_.data());

        glGenTextures(1, &sprite_texture_);
        glBindTexture(GL_TEXTURE_2D, sprite_texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        sprite_buffer_.fill(0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, 40 * 8, 16, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, sprite_buffer_.data());
    }

    VRAMView::~VRAMView()
    {
        glDeleteTextures(1, &background_texture_);
        glDeleteTextures(1, &sprite_texture_);
    }

    void VRAMView::OnGameboyChanged()
    {
        gameboy_->AddVBlankListener(this);
        sprite_buffer_.fill(0);
        vram_buffer_.fill(0);
    }

    void VRAMView::RenderBackground(bool window)
    {
        glBindTexture(GL_TEXTURE_2D, background_texture_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gandalf::TotalScreenWidth, gandalf::TotalScreenHeight, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, vram_buffer_.data());

        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::Image((void*)(intptr_t)background_texture_, ImVec2(gandalf::TotalScreenWidth * kScale, gandalf::TotalScreenHeight * kScale));

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

            constexpr float kTextureSize = gandalf::TotalScreenWidth;

            ImVec2 uv0 = ImVec2(region_x / kTextureSize, region_y / kTextureSize);
            ImVec2 uv1 = ImVec2((region_x + kTileSize) / kTextureSize, (region_y + kTileSize) / kTextureSize);
            ImGui::Image((void*)(intptr_t)background_texture_, ImVec2(kTileSize * kScaleTooltip, kTileSize * kScaleTooltip), uv0, uv1, tint_col, border_col);

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
            ImGui::TextUnformatted(text::Get(text::ID::WindowPPUTileNumber));
            ImGui::TableNextColumn();
            ImGui::Text("%2.2X", tile_data.tile_number);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(text::Get(text::ID::WindowPPUTileMap));
            ImGui::TableNextColumn();
            ImGui::Text("%4.4X", tile_data.map_address);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(text::Get(text::ID::WindowPPUTileMap)); // TODO
            ImGui::TableNextColumn();
            ImGui::Text("%4.4X", tile_data.map_offset);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(text::Get(text::ID::WindowPPUTileAddress));
            ImGui::TableNextColumn();
            ImGui::Text("%4.4X", tile_data.tile_address);
            ImGui::EndTable();

            ImGui::Separator();
            ImGui::Text("%s: %X", text::Get(text::ID::WindowPPUTileAttributes), tile_data.attributes);
            ImGui::Checkbox(text::Get(text::ID::WindowPPUTileFlipX), &tile_data.x_flip); ImGui::SameLine(); ImGui::Checkbox(text::Get(text::ID::WindowPPUTileFlipY), &tile_data.y_flip);
            ImGui::Text("%s: %d", text::Get(text::ID::WindowPPUTilePalette), tile_data.palette);
            ImGui::Text("%s: %d", text::Get(text::ID::WindowPPUTileVRAMBank), tile_data.vram_bank);
            ImGui::Checkbox(text::Get(text::ID::WindowPPUTilePriority), &tile_data.priority);

            ImGui::EndTooltip();
        }
    }

    void VRAMView::Render()
    {
        if (!gameboy_ || !debug_enabled_)
            return;

        ImGui::Begin(text::Get(text::ID::WindowPPU));

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
        if (ImGui::BeginTabBar("PPUTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem(text::Get(text::ID::WindowPPUBackground)))
            {
                current_tab_ = Tab::Background;

                RenderBackground(false);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(text::Get(text::ID::WindowPPUWindow)))
            {
                current_tab_ = Tab::Window;

                RenderBackground(true);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(text::Get(text::ID::WindowPPUTiles)))
            {
                current_tab_ = Tab::Tiles;

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(text::Get(text::ID::WindowPPUSprites)))
            {
                current_tab_ = Tab::Sprites;

                glBindTexture(GL_TEXTURE_2D, sprite_texture_);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 40 * 8, 16, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, sprite_buffer_.data());

                constexpr int kNrColumns = 16;
                if (ImGui::BeginTable(text::Get(text::ID::WindowPPUSprites), kNrColumns, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
                {
                    for (int sprite_index = 0; sprite_index < 40; ++sprite_index)
                    {
                        auto& sprite = sprite_data_.at(sprite_index);

                        ImGui::TableNextColumn();
                        ImGui::Image((void*)(intptr_t)sprite_texture_, ImVec2(8 * kSpriteScale, 16 * kSpriteScale), ImVec2(sprite_index * 8 / 320.0f, 0), ImVec2((sprite_index + 1) * 8 / 320.0f, 1.0f));
                        if (ImGui::IsItemHovered())
                        {
                            ImGuiIO& io = ImGui::GetIO();
                            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

                            ImGui::BeginTooltip();

                            ImGui::Image((void*)(intptr_t)sprite_texture_, ImVec2(8 * kScaleTooltip, 16 * kScaleTooltip), ImVec2(sprite_index * 8 / 320.0f, 0), ImVec2((sprite_index + 1) * 8 / 320.0f, 1.0f));

                            ImGui::Separator();
                            ImGui::BeginTable("Tile data", 2);

                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted("Address");
                            ImGui::TableNextColumn();
                            ImGui::Text("%4.4X", sprite.address);

                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted("X");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", sprite.x);

                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted("Y");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", sprite.y);

                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(text::Get(text::ID::WindowPPUTileNumber));
                            ImGui::TableNextColumn();
                            ImGui::Text("%2.2X", sprite.tile_number);
                            ImGui::TableNextColumn();

                            ImGui::Text("%s: %X", text::Get(text::ID::WindowPPUTileAttributes), sprite.attributes);
                            ImGui::Checkbox(text::Get(text::ID::WindowPPUTileFlipX), &sprite.x_flip); ImGui::SameLine(); ImGui::Checkbox(text::Get(text::ID::WindowPPUTileFlipY), &sprite.y_flip);
                            ImGui::Text("%s: %d", text::Get(text::ID::WindowPPUTilePalette), sprite.palette);
                            ImGui::Text("%s: %d", text::Get(text::ID::WindowPPUTileVRAMBank), sprite.vram_bank);
                            ImGui::Checkbox(text::Get(text::ID::WindowPPUTilePriority), &sprite.priority);

                            ImGui::EndTable();

                            ImGui::EndTooltip();
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text("%2X", sprite.y);
                        ImGui::Text("%2X", sprite.x);
                        ImGui::Text("%2X", sprite.tile_number);
                        ImGui::Text("%2X", sprite.attributes);
                    }
                    ImGui::EndTable();
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void VRAMView::OnVBlank()
    {
        if (!gameboy_)
            return;

        switch (current_tab_)
        {
        case Tab::Background: UpdateBackground(false); break;
        case Tab::Window: UpdateBackground(true); break;
        case Tab::Tiles: UpdateTiles(); break;
        case Tab::Sprites: UpdateSprites(); break;
        }
    }

    void VRAMView::UpdateBackground(bool window)
    {
        const auto& lcd = gameboy_->GetLCD();
        const auto& ppu = gameboy_->GetPPU();
        const gandalf::byte lcdc = gameboy_->GetLCD().GetLCDControl();
        const bool tile_map_select = window ? ((lcdc & 0x40) != 0) : ((lcdc & 0x08) != 0);

        const gandalf::word map = tile_map_select ? 0x1C00 : 0x1800;
        const gandalf::word tile_data_area = (lcdc & 0x10) == 0 ? 0x1000 : 0;

        for (int tile_y = 0; tile_y < 32; ++tile_y)
        {
            for (int tile_x = 0; tile_x < 32; ++tile_x)
            {
                TileData& tile_data = tile_data_.at(tile_y).at(tile_x);
                tile_data.map_offset = map + tile_y * 32 + tile_x;
                tile_data.tile_number = ppu.DebugReadVRam(0, tile_data.map_offset);
                tile_data.attributes = gameboy_->GetMode() == gandalf::GameboyMode::CGB ? ppu.DebugReadVRam(1, tile_data.map_offset) : 0;
                tile_data.y_flip = (tile_data.attributes & 0b1000000) != 0;
                tile_data.x_flip = (tile_data.attributes & 0b100000) != 0;
                tile_data.vram_bank = (tile_data.attributes & 0b1000) >> 3;
                tile_data.palette = tile_data.attributes & 0b111;
                tile_data.map_address = map + 0x8000;
                tile_data.priority = (tile_data.attributes & 0x80) != 0;
                tile_data.map_offset += 0x8000;

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

                        const gandalf::LCD::ABGR1555 abgr_color = lcd.GetBackgroundColor(color, tile_data.palette);
                        vram_buffer_[((tile_y * 8 + line) * gandalf::TotalScreenWidth) + tile_x * 8 + x] = abgr_color;
                    }
                }

                tile_data_.at(tile_y).at(tile_x) = tile_data;
            }
        }

        // Draw a rectangle to show the currently visible area
        const gandalf::byte left = window ? lcd.GetWX() : lcd.GetSCX();
        const gandalf::byte top = window ? lcd.GetWY() : lcd.GetSCY();
        const gandalf::byte right = left + gandalf::ScreenWidth;
        const gandalf::byte bottom = top + gandalf::ScreenHeight;
        const gandalf::LCD::ABGR1555 color = 0b11111 | 0x8000;
        for (int x = 0; x < gandalf::ScreenWidth; ++x)
        {
            vram_buffer_[gandalf::TotalScreenWidth * top + (gandalf::byte)(left + x)] = color;
            vram_buffer_[gandalf::TotalScreenWidth * bottom + (gandalf::byte)(left + x)] = color;
        }
        for (int y = 0; y < gandalf::ScreenHeight; ++y)
        {
            vram_buffer_[gandalf::TotalScreenWidth * (gandalf::byte)(top + y) + left] = color;
            vram_buffer_[gandalf::TotalScreenWidth * (gandalf::byte)(top + y) + right] = color;
        }
    }

    void VRAMView::UpdateTiles()
    {

    }

    void VRAMView::UpdateSprites()
    {
        const auto& lcd = gameboy_->GetLCD();
        const auto& ppu = gameboy_->GetPPU();
        const auto& memory = gameboy_->GetMemory();

        for (int sprite_index = 0; sprite_index < 40; ++sprite_index)
        {
            SpriteData& data = sprite_data_.at(sprite_index);
            data.address = 0xFE00 + sprite_index * 4;
            data.y = memory.Read(data.address);
            data.x = memory.Read(data.address + 1);
            data.tile_number = memory.Read(data.address + 2);
            data.attributes = memory.Read(data.address + 3);
            data.priority = (data.attributes & 0x80) != 0;
            data.y_flip = (data.attributes & 0b1000000) != 0;
            data.x_flip = (data.attributes & 0b100000) != 0;
            data.palette = gameboy_->GetMode() == gandalf::GameboyMode::CGB ? data.attributes & 0b111 : (data.attributes & 0b10000) >> 4;
            data.vram_bank = (data.attributes & 0b1000) >> 3;

            const bool sprite_size = lcd.GetLCDControl() & 0b100;
            for (int line = 0; line < 16; ++line)
            {
                std::uint8_t tile = line > 7 ? data.tile_number + 1 : data.tile_number;
                std::uint8_t tile_line = line % 8;

                const gandalf::byte tile_data_low = ppu.DebugReadVRam(data.vram_bank, tile * 16 + (data.y_flip ? (7 - tile_line) : tile_line) * 2);
                const gandalf::byte tile_data_high = ppu.DebugReadVRam(data.vram_bank, tile * 16 + (data.y_flip ? (7 - tile_line) : tile_line) * 2 + 1);

                for (int x = 0; x < 8; ++x)
                {
                    gandalf::LCD::ABGR1555 abgr_color = 0;
                    if (line < 7 || sprite_size)
                    {
                        const gandalf::byte color_bit_0 = data.x_flip ? !!((tile_data_low) & (1 << x)) : !!(tile_data_low & (1 << (7 - x)));
                        const gandalf::byte color_bit_1 = data.x_flip ? !!((tile_data_high) & (1 << x)) : !!(tile_data_high & (1 << (7 - x)));
                        const gandalf::byte color = color_bit_0 | (color_bit_1 << 1);

                        if (color != 0)
                            abgr_color = lcd.GetSpriteColor(color, data.palette);
                    }
                    sprite_buffer_[(line * (40 * 8)) + (sprite_index * 8) + x] = abgr_color;
                }
            }
        }
    }
}