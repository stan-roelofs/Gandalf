#include "settings_window.h"

#include "imgui.h"
#include "text.h"

#include <SDL.h>

namespace gui
{
    SettingsWindow::SettingsWindow(GUIContext& ctx) : ctx_(ctx), show_(true), terminated_(false), settings_copy_(ctx.GetSettings()), last_key_(std::nullopt)
    {
        ctx_.AddKeyboardHandler(this);
    }

    SettingsWindow::~SettingsWindow()
    {
        ctx_.RemoveKeyboardHandler(this);
    }

    void SettingsWindow::HandleKey(std::int32_t key, bool pressed)
    {
        if (pressed)
            last_key_ = key;
    }

    void SettingsWindow::CreateButtonRow(const char* button, std::int32_t* current_key)
    {
        assert(current_key != nullptr);

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(button);
        ImGui::TableNextColumn();

        std::string popup_name = "Settings##" + std::string(button);
        if (ImGui::Button(SDL_GetKeyName(*current_key), ImVec2(120, 0)))
        {
            ImGui::OpenPopup(popup_name.c_str());
            last_key_ = std::nullopt;
        }

        if (ImGui::BeginPopupModal(popup_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted(text::Get(text::ID::kSettingsWindowPressKey));
            if (last_key_ != std::nullopt)
            {
                if (last_key_ == SDLK_ESCAPE)
                    ImGui::CloseCurrentPopup();
                else
                {
                    *current_key = *last_key_;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    void SettingsWindow::Show()
    {
        if (show_) {
            ImGui::OpenPopup(text::Get(text::ID::kMenuSettings));
            show_ = false;
        }
        if (ImGui::BeginPopupModal(text::Get(text::ID::kMenuSettings), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTabBar("SettingsTabBar", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem(text::Get(text::ID::kSettingsWindowKeys)))
                {
                    if (ImGui::BeginTable("KeysTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                    {
                        CreateButtonRow("A", &settings_copy_.key_a);
                        CreateButtonRow("B", &settings_copy_.key_b);
                        CreateButtonRow("Start", &settings_copy_.key_start);
                        CreateButtonRow("Select", &settings_copy_.key_select);
                        CreateButtonRow("Up", &settings_copy_.key_up);
                        CreateButtonRow("Down", &settings_copy_.key_down);
                        CreateButtonRow("Left", &settings_copy_.key_left);
                        CreateButtonRow("Right", &settings_copy_.key_right);

                        ImGui::EndTable();
                    }
                }
                ImGui::EndTabBar();
            }

            if (ImGui::Button(text::Get(text::ID::kOk), ImVec2(120, 0))) {
                ctx_.GetSettings() = settings_copy_;
                terminated_ = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button(text::Get(text::ID::kCancel), ImVec2(120, 0))) {
                terminated_ = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
} // namespace gui