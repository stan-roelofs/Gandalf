#include "settings_window.h"

#include "imgui.h"
#include "text.h"

#include <gandalf/model.h>
#include <SDL.h>
#include <nfd.hpp>

namespace gui
{
    SettingsWindow::SettingsWindow(GUIContext& ctx): ctx_(ctx), show_(true), terminated_(false), settings_copy_(ctx.GetSettings()), last_key_(std::nullopt)
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
            ImGui::TextUnformatted(text::Get(text::ID::SettingsWindowPressKey));
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
            ImGui::OpenPopup(text::Get(text::ID::MenuSettings));
            show_ = false;
        }
        if (ImGui::BeginPopupModal(text::Get(text::ID::MenuSettings), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTabBar("SettingsTabBar", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem(text::Get(text::ID::SettingsWindowGeneral)))
                {
                    if (ImGui::BeginCombo(text::Get(text::ID::kSettingsEmulatedModel), gandalf::GetModelName(static_cast<gandalf::Model>(settings_copy_.emulated_model)).c_str()))
                    {
                        for (int model = 0; model < static_cast<int>(gandalf::Model::LAST); ++model)
                        {
                            const bool is_selected = (static_cast<int>(settings_copy_.emulated_model) == model);
                            if (ImGui::Selectable(gandalf::GetModelName(static_cast<gandalf::Model>(model)).c_str(), is_selected))
                                settings_copy_.emulated_model = model;

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::TextUnformatted(text::Get(text::ID::SettingsWindowBootROMLocation));
                    ImGui::SameLine();
                    ImGui::TextUnformatted(settings_copy_.boot_rom_location.c_str());
                    ImGui::SameLine();

                    if (ImGui::Button(text::Get(text::ID::SettingsWindowBootROMSelect), ImVec2(120, 0)))
                    {
                        NFD::UniquePath path;
                        auto result = NFD::OpenDialog(path);
                        if (result == NFD_OKAY)
                            settings_copy_.boot_rom_location = path.get();
                    };

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(text::Get(text::ID::SettingsWindowKeys)))
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
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            if (ImGui::Button(text::Get(text::ID::Ok), ImVec2(120, 0))) {
                ctx_.GetSettings() = settings_copy_;
                terminated_ = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button(text::Get(text::ID::Cancel), ImVec2(120, 0))) {
                terminated_ = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
} // namespace gui