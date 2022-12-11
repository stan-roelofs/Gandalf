#include "text.h"

#include <cassert>
#include <map>
#include <stdexcept>

namespace gui
{
    namespace text
    {
        using TextMap = std::map<ID, const char*>;

        static const TextMap texts_ = {
            {ID::kAppName, "Gandalf"},
            {ID::kOk, "OK"},
            {ID::kCancel, "Cancel"},
            {ID::kPause, "Pause"},

            {ID::kSelectBootROM, "Select Boot ROM"},
            {ID::kErrorLoadBootROM, "Error while loading Gameboy Boot ROM"},
            {ID::kErrorLoad, "Error loading file"},

            {ID::kMenuFile, "File"},
            {ID::kMenuFileOpenROM, "Open ROM"},
            {ID::kMenuFileRecentROMs, "Recent ROMs"},

            {ID::kMenuSettings, "Settings"},
            {ID::kMenuSettingsOpen, "All Settings"},

            {ID::kMenuEmulation, "Emulation"},

            {ID::kMenuView, "View"},
            {ID::kMenuViewDebug, "Debug"},

            {ID::kSettingsWindowKeys, "Keys"},
            {ID::kSettingsWindowPressKey, "Press a key or escape to cancel"},
            {ID::kSettingsWindowGeneral, "General"},
            {ID::kSettingsWindowBootROMLocation, "Boot ROM location"},
            {ID::kSettingsWindowBootROMSelect, "Select"},

            {ID::kWindowAPU, "APU"},
            {ID::kWindowAPUSquare1Enable, "Enable Square wave 1 channel"},
            {ID::kWindowAPUSquare2Enable, "Enable Square wave 2 channel"},
            {ID::kWindowAPUWaveEnable, "Enable Wave channel"},
            {ID::kWindowAPUNoiseEnable, "Enable Noise channel"},

            {ID::kWindowCartridge, "Cartridge"},
            {ID::kWindowCartridgeTitle, "Title"},
            {ID::kWindowCartridgeManufacturer, "Manufacturer"},
            {ID::kWindowCartridgeLicensee, "Licensee"},
            {ID::kWindowCartridgeROMSize, "ROM Size"},
            {ID::kWindowCartridgeRAMSize, "RAM Size"},
            {ID::kWindowCartridgeCGB, "CGB"},
            {ID::kWindowCartridgeSGB, "SGB"},
            {ID::kWindowCartridgeType, "Type"},
            {ID::kWindowCartridgeDestination, "Destination"},

            {ID::kWindowCPU, "CPU"},
            {ID::kWindowCPUStep, "Step"},
            {ID::kWindowCPULimitFPS, "Limit FPS"},
            {ID::kWindowCPURegisters, "Registers"},
            {ID::kWindowCPUDebugger, "Debugger"},

            {ID::kWindowGameboy, "Gameboy"},
            {ID::kWindowGameboyScale, "Scale"},

            {ID::kWindowMemory, "Memory"},
            {ID::kWindowMemoryScrollToAddress, "Scroll to address"},
            {ID::kWindowMemoryViewer, "Viewer"},
            {ID::kWindowMemoryViewerAddress, "Address"},
            {ID::kWindowMemoryViewerAddressOwner, "Owner"},
            {ID::kWindowMemoryViewerAddressValue, "Value"},

            {ID::kWindowPPU, "PPU"},
            {ID::kWindowPPUBackgroundMap, "Background map"},
            {ID::kWindowPPUTileNumber, "Tile number"},
            {ID::kWindowPPUTileMap, "Tile map"},
            {ID::kWindowPPUTileAddress, "Tile address"},
            {ID::kWindowPPUTileAttributes, "Tile attributes"},
            {ID::kWindowPPUTileFlipX, "Flip X"},
            {ID::kWindowPPUTileFlipY, "Flip Y"},
            {ID::kWindowPPUTilePalette, "Palette"},
            {ID::kWindowPPUTileVRAMBank, "VRAM bank"},
            {ID::kWindowPPUTilePriority, "Priority"},
            {ID::kWindowPPUTiles, "Tiles"},

            {ID::kWindowIO, "IO"},
        };

        const char* Get(ID id)
        {
            try {
                return texts_.at(id);
            }
            catch (std::out_of_range&)
            {
                assert(false);
                return "<MISSING TEXT>";
            }
        }
    } // namespace text
} // namespace gui