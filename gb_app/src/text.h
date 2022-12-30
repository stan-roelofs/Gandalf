#ifndef __GB_APP_TEXT_H
#define __GB_APP_TEXT_H

namespace gui
{
    namespace text
    {
        enum class ID
        {
            kAppName,
            kOk,
            kCancel,
            kPause,

            // Errors
            kErrorLoadBootROM,
            kSelectBootROM,
            kErrorLoad,

            // File Menu
            kMenuFile,
            kMenuFileOpenROM,
            kMenuFileRecentROMs,

            // Settings Menu
            kMenuSettings,
            kMenuSettingsOpen,

            // Emulation Menu
            kMenuEmulation,

            // View Menu
            kMenuView,
            kMenuViewDebug,

            // Settings window
            kSettingsWindowKeys,
            kSettingsWindowPressKey,
            kSettingsWindowGeneral,
            kSettingsWindowBootROMLocation,
            kSettingsWindowBootROMSelect,

            // APU window
            kWindowAPU,
            kWindowAPUSquare1Enable,
            kWindowAPUSquare2Enable,
            kWindowAPUWaveEnable,
            kWindowAPUNoiseEnable,

            // Cartridge window
            kWindowCartridge,
            kWindowCartridgeTitle,
            kWindowCartridgeManufacturer,
            kWindowCartridgeLicensee,
            kWindowCartridgeROMSize,
            kWindowCartridgeRAMSize,
            kWindowCartridgeCGB,
            kWindowCartridgeSGB,
            kWindowCartridgeType,
            kWindowCartridgeDestination,

            // CPU window
            kWindowCPU,
            kWindowCPUStep,
            kWindowCPULimitFPS,
            kWindowCPURegisters,
            kWindowCPUDebugger,

            // Gameboy window
            kWindowGameboy,
            kWindowGameboyScale,

            // Memory window
            kWindowMemory,
            kWindowMemoryScrollToAddress,
            kWindowMemoryViewer,
            kWindowMemoryViewerAddress,
            kWindowMemoryViewerAddressOwner,
            kWindowMemoryViewerAddressValue,

            // PPU window
            kWindowPPU,
            kWindowPPUBackgroundMap,
            kWindowPPUTileNumber,
            kWindowPPUTileMap,
            kWindowPPUTileAddress,
            kWindowPPUTileAttributes,
            kWindowPPUTileFlipX,
            kWindowPPUTileFlipY,
            kWindowPPUTilePalette,
            kWindowPPUTileVRAMBank,
            kWindowPPUTilePriority,
            kWindowPPUTiles,
            kWindowPPUSprites,

            // IO window
            kWindowIO,
        };

        const char* Get(ID id);
    }
} // namespace gui

#endif