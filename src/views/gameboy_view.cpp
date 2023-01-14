#include "gameboy_view.h"

#include <imgui.h>

#include "../text.h"

namespace gui
{
    GameboyView::GameboyView(GUIContext& context):
        back_buffer_(std::make_unique<gandalf::LCD::VideoBuffer>()),
        front_buffer_(std::make_unique<gandalf::LCD::VideoBuffer>()),
        scale_(5),
        context_(context)
    {
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, gandalf::kScreenWidth, gandalf::kScreenHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, front_buffer_->data());
    }

    GameboyView::~GameboyView()
    {
        glDeleteTextures(1, &texture_);
    }

    void GameboyView::OnGameboyChanged()
    {
        gameboy_->AddVBlankListener(this);
    }

    void GameboyView::Render()
    {
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gandalf::kScreenWidth, gandalf::kScreenHeight, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, front_buffer_->data());

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(text::Get(text::ID::WindowGameboy));
        ImGui::SliderInt(text::Get(text::ID::WindowGameboyScale), &context_.GetSettings().scale, 1, 5);
        ImGui::Image((void*)(intptr_t)texture_, ImVec2(static_cast<float>(gandalf::kScreenWidth * context_.GetSettings().scale), static_cast<float>(gandalf::kScreenHeight * context_.GetSettings().scale)));
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GameboyView::OnVBlank()
    {
        std::swap(front_buffer_, back_buffer_);
        std::copy(gameboy_->GetLCD().GetVideoBuffer().begin(), gameboy_->GetLCD().GetVideoBuffer().end(), back_buffer_->begin());
    }
}