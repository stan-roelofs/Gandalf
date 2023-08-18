#ifndef __GB_APP_TEXT_H
#define __GB_APP_TEXT_H

namespace gui
{
    namespace text
    {
        enum class ID
        {
            AppName,
            Ok,
            Cancel,
            Pause,

            // Errors
            ErrorLoad,
            ErrorSaveState,
            ErrorLoadState,

            // File Menu
            MenuFile,
            MenuFileOpenROM,
            MenuFileRecentROMs,
            MenuFileSaveState,
            MenuFileQuickSaveState,
            MenuFileLoadState,
            MenuFileQuickLoadState,

            // Settings Menu
            MenuSettings,
            MenuSettingsOpen,

            // Emulation Menu
            MenuEmulation,

            // View Menu
            MenuView,
            MenuViewDebug,

            // Settings window
            SettingsWindowKeys,
            SettingsWindowPressKey,
            SettingsWindowGeneral,
            SettingsWindowEmulatedModel,

            // APU window
            WindowAPU,
            WindowAPUSquare1Enable,
            WindowAPUSquare2Enable,
            WindowAPUWaveEnable,
            WindowAPUNoiseEnable,

            // Cartridge window
            WindowCartridge,
            WindowCartridgeTitle,
            WindowCartridgeManufacturer,
            WindowCartridgeLicensee,
            WindowCartridgeROMSize,
            WindowCartridgeRAMSize,
            WindowCartridgeCGB,
            WindowCartridgeSGB,
            WindowCartridgeType,
            WindowCartridgeDestination,

            // CPU window
            WindowCPU,
            WindowCPUStep,
            WindowCPULimitFPS,
            WindowCPUDebugger,
            WindowCPUJump,

            // Gameboy window
            WindowGameboy,
            WindowGameboyScale,

            // Memory window
            WindowMemory,
            WindowMemoryScrollToAddress,
            WindowMemoryViewer,
            WindowMemoryViewerAddress,
            WindowMemoryViewerAddressOwner,
            WindowMemoryViewerAddressValue,

            // PPU window
            WindowPPU,
            WindowPPUBackground,
            WindowPPUWindow,
            WindowPPUTileNumber,
            WindowPPUTileMap,
            WindowPPUTileMapOffset,
            WindowPPUTileAddress,
            WindowPPUTileAttributes,
            WindowPPUTileFlipX,
            WindowPPUTileFlipY,
            WindowPPUTilePalette,
            WindowPPUTileVRAMBank,
            WindowPPUTilePriority,
            WindowPPUTiles,
            WindowPPUSprites,

            // IO window
            WindowIO,
        };

        const char* Get(ID id);
    }
} // namespace gui

#endif