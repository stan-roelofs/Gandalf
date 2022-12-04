#include "settings.h"

#include "SDL_keycode.h"

namespace gui
{
    void to_json(json& j, const Settings& s)
    {
        j = json{
            {"recent_roms", s.recent_roms},
            {"boot_rom_location", s.boot_rom_location},
            {"show_debug", s.show_debug},
            {"key_up", s.key_up},
            {"key_down", s.key_down},
            {"key_left", s.key_left},
            {"key_right", s.key_right},
            {"key_a", s.key_a},
            {"key_b", s.key_b},
            {"key_start", s.key_start},
            {"key_select", s.key_select}
        };
    }

    void from_json(const json& j, Settings& s)
    {
        s.recent_roms = j.at("recent_roms");
        s.boot_rom_location = j.at("boot_rom_location");
        s.show_debug = j.at("show_debug");
        s.key_up = j.at("key_up");
        s.key_down = j.at("key_down");
        s.key_left = j.at("key_left");
        s.key_right = j.at("key_right");
        s.key_a = j.at("key_a");
        s.key_b = j.at("key_b");
        s.key_start = j.at("key_start");
        s.key_select = j.at("key_select");
    }

    Settings::Settings()
    {
        recent_roms = {};
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