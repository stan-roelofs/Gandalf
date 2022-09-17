#ifndef __GANDALF_CONSTANTS_H
#define __GANDALF_CONSTANTS_H

#include "types.h"

namespace gandalf {
    constexpr word kVBK = 0xFF4F;
    constexpr word kLCDC = 0xFF40;
    constexpr word kLY = 0xFF44;
    constexpr word kLYC = 0xFF45;
    constexpr word kSTAT = 0xFF41;
    constexpr word kSCY = 0xFF42;
    constexpr word kSCX = 0xFF43;
    constexpr word kWY = 0xFF4A;
    constexpr word kWX = 0xFF4B;
    constexpr word kBGP = 0xFF47;
    constexpr word kOBP0 = 0xFF48;
    constexpr word kOBP1 = 0xFF49;
    constexpr word kBCPS = 0xFF68;
    constexpr word kBCPD = 0xFF69;
    constexpr word kOCPS = 0xFF6A;
    constexpr word kOCPD = 0xFF6B;

    // Interrupts
    constexpr word kIF = 0xFF0F;
    constexpr word kIE = 0xFFFF;

    // Timer
    constexpr word kDIV = 0xFF04;
    constexpr word kTIMA = 0xFF05;
    constexpr word kTMA = 0xFF06;
    constexpr word kTAC = 0xFF07;

    // Joypad
    constexpr word kP1 = 0xFF00;

    // Serial
    constexpr word kSB = 0xFF01;
    constexpr word kSC = 0xFF02;

    // DMA
    constexpr word kDMA = 0xFF46;
    constexpr word kHDMA1 = 0xFF51;
    constexpr word kHDMA2 = 0xFF52;
    constexpr word kHDMA3 = 0xFF53;
    constexpr word kHDMA4 = 0xFF54;
    constexpr word kHDMA5 = 0xFF55;

    // Sound
    constexpr word kNR10 = 0xFF10;
    constexpr word kNR11 = 0xFF11;
    constexpr word kNR12 = 0xFF12;
    constexpr word kNR13 = 0xFF13;
    constexpr word kNR14 = 0xFF14;

    constexpr word kNR20 = 0xFF15;
    constexpr word kNR21 = 0xFF16;
    constexpr word kNR22 = 0xFF17;
    constexpr word kNR23 = 0xFF18;
    constexpr word kNR24 = 0xFF19;

    constexpr word kNR30 = 0xFF1A;
    constexpr word kNR31 = 0xFF1B;
    constexpr word kNR32 = 0xFF1C;
    constexpr word kNR33 = 0xFF1D;
    constexpr word kNR34 = 0xFF1E;

    constexpr word kNR40 = 0xFF1F;
    constexpr word kNR41 = 0xFF20;
    constexpr word kNR42 = 0xFF21;
    constexpr word kNR43 = 0xFF22;
    constexpr word kNR44 = 0xFF23;

    constexpr word kNR50 = 0xFF24;
    constexpr word kNR51 = 0xFF25;
    constexpr word kNR52 = 0xFF26;

    constexpr word kBANK = 0xFF50;

    // CGB
    constexpr word kKEY1 = 0xFF4D;
    constexpr word kRP = 0xFF56;
    constexpr word kOPRI = 0xFF6C;
    constexpr word kSVBK = 0xFF70;

    // Flags
    constexpr byte kZFlagMask = 0x80;
    constexpr byte kNFlagMask = 0x40;
    constexpr byte kHFlagMask = 0x20;
    constexpr byte kCFlagMask = 0x10;

    // Interrupts
    constexpr byte kVBlankInterruptMask = 0x01;
    constexpr byte kLCDInterruptMask = 0x02;
    constexpr byte kTimerInterruptMask = 0x04;
    constexpr byte kSerialInterruptMask = 0x08;
    constexpr byte kJoypadInterruptMask = 0x10;

    //
    constexpr int kScreenHeight = 144;
    constexpr int kScreenWidth = 160;
    constexpr int kCPUFrequency = 4194304; // MHz

    enum class GameboyMode
    {
        DMG,
        CGB
    };

} // namespace gandalf

#endif
