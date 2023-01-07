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
            {ID::AppName, "Gandalf"},
            {ID::Ok, "OK"},
            {ID::Cancel, "Cancel"},
            {ID::Pause, "Pause"},

            {ID::ErrorLoad, "Error loading file"},

            {ID::MenuFile, "File"},
            {ID::MenuFileOpenROM, "Open ROM"},
            {ID::MenuFileRecentROMs, "Recent ROMs"},

            {ID::MenuSettings, "Settings"},
            {ID::MenuSettingsOpen, "All Settings"},

            {ID::MenuEmulation, "Emulation"},

            {ID::MenuView, "View"},
            {ID::MenuViewDebug, "Debug"},

            {ID::SettingsWindowKeys, "Keys"},
            {ID::SettingsWindowPressKey, "Press a key or escape to cancel"},
            {ID::SettingsWindowGeneral, "General"},
            {ID::SettingsWindowEmulatedModel, "Emulated model"},

            {ID::WindowAPU, "APU"},
            {ID::WindowAPUSquare1Enable, "Enable Square wave 1 channel"},
            {ID::WindowAPUSquare2Enable, "Enable Square wave 2 channel"},
            {ID::WindowAPUWaveEnable, "Enable Wave channel"},
            {ID::WindowAPUNoiseEnable, "Enable Noise channel"},

            {ID::WindowCartridge, "Cartridge"},
            {ID::WindowCartridgeTitle, "Title"},
            {ID::WindowCartridgeManufacturer, "Manufacturer"},
            {ID::WindowCartridgeLicensee, "Licensee"},
            {ID::WindowCartridgeROMSize, "ROM Size"},
            {ID::WindowCartridgeRAMSize, "RAM Size"},
            {ID::WindowCartridgeCGB, "CGB"},
            {ID::WindowCartridgeSGB, "SGB"},
            {ID::WindowCartridgeType, "Type"},
            {ID::WindowCartridgeDestination, "Destination"},

            {ID::WindowCPU, "CPU"},
            {ID::WindowCPUStep, "Step"},
            {ID::WindowCPULimitFPS, "Limit FPS"},
            {ID::WindowCPURegisters, "Registers"},
            {ID::WindowCPUDebugger, "Debugger"},

            {ID::WindowGameboy, "Gameboy"},
            {ID::WindowGameboyScale, "Scale"},

            {ID::WindowMemory, "Memory"},
            {ID::WindowMemoryScrollToAddress, "Scroll to address"},
            {ID::WindowMemoryViewer, "Viewer"},
            {ID::WindowMemoryViewerAddress, "Address"},
            {ID::WindowMemoryViewerAddressOwner, "Owner"},
            {ID::WindowMemoryViewerAddressValue, "Value"},

            {ID::WindowPPU, "PPU"},
            {ID::WindowPPUBackgroundMap, "Background map"},
            {ID::WindowPPUTileNumber, "Tile number"},
            {ID::WindowPPUTileMap, "Tile map"},
            {ID::WindowPPUTileAddress, "Tile address"},
            {ID::WindowPPUTileAttributes, "Tile attributes"},
            {ID::WindowPPUTileFlipX, "Flip X"},
            {ID::WindowPPUTileFlipY, "Flip Y"},
            {ID::WindowPPUTilePalette, "Palette"},
            {ID::WindowPPUTileVRAMBank, "VRAM bank"},
            {ID::WindowPPUTilePriority, "Priority"},
            {ID::WindowPPUTiles, "Tiles"},
            {ID::WindowPPUSprites, "Sprites"},

            {ID::WindowIO, "IO"},
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