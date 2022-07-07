#include <gandalf/ppu.h>

#include <gandalf/constants.h>

namespace {
    const int kTicksPerLine = 456;
    const int kLinesPerFrame = 154;

    enum class PPUMode {
        HBlank,
        VBlank,
        OamSearch,
        PixelTransfer
    };

    PPUMode GetMode(gandalf::byte status)
    {
        return static_cast<PPUMode>(status & 0b11);
    }

    void SetMode(PPUMode mode, gandalf::byte& status)
    {
        status = (status & 0b11111100) | static_cast<gandalf::byte>(mode);
    }
}

namespace gandalf {
    PPU::PPU(Bus& bus, LCD& lcd) : Bus::AddressHandler("PPU"), bus_(bus), lcd_(lcd), line_ticks_(0)
    {
        vram_.fill(0xFF);
        oam_.fill(0xFF);
    }

    PPU::~PPU() = default;

    // TODO stat interrupts can block eachother
    void PPU::Tick()
    {
        ++line_ticks_;
        byte& stat = lcd_.GetLCDStatus();

        switch (GetMode(lcd_.GetLCDStatus()))
        {
        case PPUMode::OamSearch:
            if (line_ticks_ >= 80)
                SetMode(PPUMode::PixelTransfer, stat);
            break;
        case PPUMode::PixelTransfer:
            if (line_ticks_ >= 252) {
                SetMode(PPUMode::HBlank, stat);
                if (stat & 0b00001000)
                    bus_.Write(kIE, bus_.Read(kIE) | kLCDInterruptMask);
            }
            break;
        case PPUMode::HBlank:
            if (line_ticks_ >= kTicksPerLine) {
                IncrementLY();

                if (lcd_.GetLY() >= kScreenHeight) {
                    SetMode(PPUMode::VBlank, stat);

                    bus_.Write(kIE, bus_.Read(kIE) | kVBlankInterruptMask);

                    if (stat & 0b00010000)
                        bus_.Write(kIE, bus_.Read(kIE) | kLCDInterruptMask);
                }
                else
                    SetMode(PPUMode::OamSearch, stat);

                line_ticks_ = 0;
            }
            break;
        case PPUMode::VBlank:
            if (line_ticks_ >= 456) {
                IncrementLY();

                byte& ly = lcd_.GetLY();
                if (ly >= kLinesPerFrame) {
                    SetMode(PPUMode::OamSearch, stat);
                    ly = 0;
                }

                line_ticks_ = 0;
            }
            break;
        }
    }

    byte PPU::Read(word address) const
    {
        if (address >= 0x8000 && address < 0xA000)
        {
            return vram_[address - 0x8000];
        }
        else if (address >= 0xFE00 && address < 0xFEA0)
        {
            return oam_[address - 0xFE00];
        }
        throw Exception("Invalid PPU address : " + std::to_string(address));
    }

    void PPU::Write(word address, byte value)
    {
        if (address >= 0x8000 && address < 0xA000)
        {
            vram_[address - 0x8000] = value;
        }
        else if (address >= 0xFE00 && address < 0xFEA0)
        {
            oam_[address - 0xFE00] = value;
        }
        else
            throw Exception("Invalid PPU address : " + std::to_string(address));
    }

    std::set<word> PPU::GetAddresses() const
    {
        std::set<word> result;
        for (word i = 0x8000; i < 0xA000; ++i)
            result.insert(i);

        for (word i = 0xFE00; i < 0xFEA0; ++i)
            result.insert(i);

        return result;
    }

    void PPU::IncrementLY()
    {
        byte& ly = lcd_.GetLY();
        byte& lyc = lcd_.GetLYC();
        byte& stat = lcd_.GetLCDStatus();

        ++ly;

        if (ly == lyc) {
            // Set LY==LYC coincidence flag
            stat |= 0b100;

            if (stat & 0b01000000) {
                bus_.Write(kIE, bus_.Read(kIE) | kLCDInterruptMask);
            }
        }
        else {
            stat &= 0b11111011;
        }
    }
} // namespace gandalf