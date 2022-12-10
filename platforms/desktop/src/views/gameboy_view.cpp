#include "gameboy_view.h"

#include <imgui.h>

#include "../text.h"

namespace gui
{
    GameboyView::GameboyView(SDL_Renderer& renderer) :
        back_buffer_(std::make_unique<gandalf::LCD::VideoBuffer>()),
        front_buffer_(std::make_unique<gandalf::LCD::VideoBuffer>()),
        scale_(5)
    {
        texture_ = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_BGR555,
            SDL_TEXTUREACCESS_STREAMING,
            gandalf::kScreenWidth, gandalf::kScreenHeight);

        if (!texture_)
            throw std::runtime_error("Failed to create texture!");
    }

    GameboyView::~GameboyView()
    {
        SDL_DestroyTexture(texture_);
    }

    void GameboyView::SetGameboy(std::shared_ptr<gandalf::Gameboy> gameboy)
    {
        GUIElement::SetGameboy(gameboy);
        gameboy_->GetPPU().AddVBlankListener(this);
    }

    void GameboyView::Render()
    {
        SDL_UpdateTexture(texture_, nullptr, front_buffer_.get(), gandalf::kScreenWidth * sizeof(gandalf::LCD::BGR555));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(text::Get(text::ID::kWindowGameboy));
        ImGui::SliderInt(text::Get(text::ID::kWindowGameboyScale), &scale_, 1, 5);
        ImGui::Image(texture_, ImVec2(gandalf::kScreenWidth * scale_, gandalf::kScreenHeight * scale_));
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GameboyView::OnVBlank()
    {
        std::swap(front_buffer_, back_buffer_);
        std::copy(gameboy_->GetLCD().GetVideoBuffer().begin(), gameboy_->GetLCD().GetVideoBuffer().end(), back_buffer_->begin());
    }
}