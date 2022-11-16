#include "settings_window.h"

#include "imgui.h"
#include "text.h"

namespace gui
{
    SettingsWindow::SettingsWindow(settings::Settings& settings) : settings_(settings), show_(true), terminated_(false)
    {
    }

    SettingsWindow::~SettingsWindow() = default;

    void CreateButtonRow(const char* button)
    {
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(button);
        ImGui::TableNextColumn();
        ImGui::Button(button);
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
                        CreateButtonRow("A");
                        CreateButtonRow("B");
                        CreateButtonRow("Start");
                        CreateButtonRow("Select");
                        CreateButtonRow("Up");
                        CreateButtonRow("Down");
                        CreateButtonRow("Left");
                        CreateButtonRow("Right");

                        ImGui::EndTable();
                    }
                }
                ImGui::EndTabBar();
            }

            if (ImGui::Button(text::Get(text::ID::kOk), ImVec2(120, 0))) {
                settings_ = settings_copy_;
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