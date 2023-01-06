#include "settings.h"

#include <gandalf/model.h>

#include "SDL_keycode.h"

namespace gui
{
    void to_json(json& j, const Settings& s)
    {
        j = json{
            {"recent_roms", s.recent_roms},
            {"emulated_model", s.emulated_model},
            {"boot_rom_location", s.boot_rom_location},
            {"show_debug", s.show_debug},
            {"key_up", s.key_up},
            {"key_down", s.key_down},
            {"key_left", s.key_left},
            {"key_right", s.key_right},
            {"key_a", s.key_a},
            {"key_b", s.key_b},
            {"key_start", s.key_start},
            {"key_select", s.key_select},
            {"window_width", s.window_width},
            {"window_height", s.window_height},
            {"scale", s.scale}
        };
    }

    void from_json(const json& j, Settings& s)
    {
        Settings default_settings;

        s.recent_roms = j.value("recent_roms", default_settings.recent_roms);
        s.emulated_model = j.value("emulated_model", default_settings.emulated_model);
        s.boot_rom_location = j.value("boot_rom_location", default_settings.boot_rom_location);
        s.show_debug = j.value("show_debug", default_settings.show_debug);
        s.key_up = j.value("key_up", default_settings.key_up);
        s.key_down = j.value("key_down", default_settings.key_down);
        s.key_left = j.value("key_left", default_settings.key_left);
        s.key_right = j.value("key_right", default_settings.key_right);
        s.key_a = j.value("key_a", default_settings.key_a);
        s.key_b = j.value("key_b", default_settings.key_b);
        s.key_start = j.value("key_start", default_settings.key_start);
        s.key_select = j.value("key_select", default_settings.key_select);
        s.window_width = j.value("window_width", default_settings.window_width);
        s.window_height = j.value("window_height", default_settings.window_height);
        s.scale = j.value("scale", default_settings.scale);
    }

    Settings::Settings()
    {
        recent_roms = {};
        emulated_model = gandalf::Model::CGB;
        boot_rom_location = "";
        show_debug = false;
        key_up = SDLK_UP;
        key_down = SDLK_DOWN;
        key_left = SDLK_LEFT;
        key_right = SDLK_RIGHT;
        key_a = SDLK_z;
        key_b = SDLK_x;
        key_start = SDLK_RETURN;
        key_select = SDLK_TAB;
        window_width = 800;
        window_height = 800;
        scale = 5;
    }

    namespace settings {
        bool Read(const std::filesystem::path& path, Settings& settings)
        {
            std::ifstream ifs(path);
            std::string str(std::istreambuf_iterator<char>{ifs}, {});

            json j;
            try {
                j = json::parse(str);
                settings = j;
            }
            catch (json::parse_error& err)
            {
                std::cerr << "Parse error " << err.what() << std::endl;
                return false;
            }

            return true;
        }

        bool Write(const std::filesystem::path& path, const Settings& settings)
        {
            std::ofstream ofs(path);
            json j = settings;
            ofs << j;
            ofs.close();
            return true;
        }
    }
}