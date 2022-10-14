#include "vram_view.h"

#include <imgui.h>

namespace
{
    const int kTileSize = 8;
    const int scale = 2;
}

namespace gui
{
    VRAMView::VRAMView(const bool& debug_enabled, SDL_Renderer& renderer) : debug_enabled_(debug_enabled), visible_(false)
    {
        texture_ = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, gandalf::kTotalScreenWidth, gandalf::kTotalScreenHeight);
        if (!texture_)
            throw std::runtime_error("Failed to create texture!");
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
        if (!gameboy_ || !visible_)
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

                ImGui::Image(texture_, ImVec2(gandalf::kTotalScreenWidth * scale, gandalf::kTotalScreenHeight * scale));

                if (ImGui::IsItemHovered())
                {
                    ImGuiIO& io = ImGui::GetIO();
                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
                    float my_tex_w = (float)gandalf::kTotalScreenWidth * scale;
                    float my_tex_h = (float)gandalf::kTotalScreenHeight * scale;

                    ImGui::BeginTooltip();
                    float region_sz = 32.0f;
                    float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                    float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                    float zoom = 4.0f;
                    if (region_x < 0.0f) { region_x = 0.0f; }
                    else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
                    if (region_y < 0.0f) { region_y = 0.0f; }
                    else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }

                    region_x = ((int)region_x / kTileSize) * kTileSize;
                    region_y = ((int)region_y / kTileSize) * kTileSize;

                    ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                    ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                    ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
                    ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
                    ImGui::Image(texture_, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
                ImGui::Text("test");

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
                const gandalf::byte tile_index = ppu.DebugReadVRam(0, map + tile_y * 32 + tile_x);
                const gandalf::byte tile_attributes = gameboy_->GetMode() == gandalf::GameboyMode::CGB ? ppu.DebugReadVRam(1, map + tile_y * 32 + tile_x) : 0;
                const int tile_offset = (tile_data_area == 0 ? tile_index : (gandalf::signed_byte)(tile_index));

                for (int line = 0; line < 8; ++line)
                {
                    const bool flip_y = (tile_attributes & 0b1000000) != 0;
                    const int vram_bank = (tile_attributes & 0b1000) >> 3;
                    const gandalf::byte tile_data_low = ppu.DebugReadVRam(vram_bank, tile_data_area + tile_offset * 16 + (flip_y ? (7 - line) : line) * 2);
                    const gandalf::byte tile_data_high = ppu.DebugReadVRam(vram_bank, tile_data_area + tile_offset * 16 + (flip_y ? (7 - line) : line) * 2 + 1);

                    for (int x = 0; x < 8; ++x)
                    {
                        const bool flip_x = (tile_attributes & 0b100000) != 0;
                        const gandalf::byte color_bit_0 = flip_x ? !!((tile_data_low) & (1 << x)) : !!(tile_data_low & (1 << (7 - x)));
                        const gandalf::byte color_bit_1 = flip_x ? !!((tile_data_high) & (1 << x)) : !!(tile_data_high & (1 << (7 - x)));
                        const gandalf::byte color = color_bit_0 | (color_bit_1 << 1);

                        const gandalf::LCD::BGR555 bgr_color = lcd.GetBackgroundColor(color, tile_attributes & 0b111);
                        vram_buffer_[((tile_y * 8 + line) * gandalf::kTotalScreenWidth) + tile_x * 8 + x] = bgr_color;
                    }
                }
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