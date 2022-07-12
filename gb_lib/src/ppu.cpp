#include <gandalf/ppu.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/util.h>

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
        return static_cast<PPUMode>(status & 0x3);
    }

    void SetMode(PPUMode mode, gandalf::byte& status)
    {
        status = (status & 0xFC) | static_cast<gandalf::byte>(mode);
    }
}

namespace gandalf {
    PPU::PPU(Bus& bus, LCD& lcd) : Bus::AddressHandler("PPU"), bus_(bus), lcd_(lcd), line_ticks_(0), vblank_listener_(nullptr), pipeline_(lcd_, vram_)
    {
        vram_.fill((byte)std::rand());
        oam_.fill((byte)std::rand());
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
            if (line_ticks_ >= 80) {
                pipeline_.Reset();
                SetMode(PPUMode::PixelTransfer, stat);
            }
            break;
        case PPUMode::PixelTransfer:
            pipeline_.Process(line_ticks_);

            if (pipeline_.Done()) {
                SetMode(PPUMode::HBlank, stat);
                if (stat & 0x8)
                    bus_.Write(kIF, bus_.Read(kIF) | kLCDInterruptMask);
            }
            break;
        case PPUMode::HBlank:
            if (line_ticks_ >= kTicksPerLine) {
                IncrementLY();

                if (lcd_.GetLY() >= kScreenHeight) {
                    SetMode(PPUMode::VBlank, stat);

                    bus_.Write(kIF, bus_.Read(kIF) | kVBlankInterruptMask);

                    if (stat & 0x10)
                        bus_.Write(kIF, bus_.Read(kIF) | kLCDInterruptMask);

                    if (vblank_listener_)
                        vblank_listener_->OnVBlank();
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
        assert(BETWEEN(address, 0x8000, 0xA000) || BETWEEN(address, 0xFE00, 0xFEA0));

        if (address >= 0x8000 && address < 0xA000)
            return vram_[address - 0x8000];
        else if (address >= 0xFE00 && address < 0xFEA0)
            return oam_[address - 0xFE00];
        return 0xFF;
    }

    void PPU::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0x8000, 0xA000) || BETWEEN(address, 0xFE00, 0xFEA0));

        if (address >= 0x8000 && address < 0xA000)
            vram_[address - 0x8000] = value;
        else if (address >= 0xFE00 && address < 0xFEA0)
            oam_[address - 0xFE00] = value;
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
            stat |= 0x4;

            if (stat & 0x40) {
                bus_.Write(kIF, bus_.Read(kIF) | kLCDInterruptMask);
            }
        }
        else {
            stat &= 0xFB;
        }
    }

    PPU::Pipeline::Pipeline(LCD& lcd, VRAM& vram) : lcd_(lcd), vram_(vram)
    {
        Reset();
    }

    PPU::Pipeline::~Pipeline() = default;

    void PPU::Pipeline::Reset()
    {
        fetcher_state_ = FetcherState::kFetchTileSleep;
        pixels_pushed_ = 0;
        fetch_x_ = 0;
        fetch_y_ = lcd_.GetLY() + lcd_.GetSCY();

        std::queue<Pixel> empty;
        background_fifo_.swap(empty);
        sprite_fifo_.swap(empty);
    }

    bool PPU::Pipeline::Done() const
    {
        return pixels_pushed_ == kScreenWidth;
    }

    void PPU::Pipeline::Process(int line_ticks)
    {
        if ((line_ticks & 0x1) == 0)
            StateMachine();

        RenderPixel();
    }

    void PPU::Pipeline::StateMachine()
    {
        switch (fetcher_state_)
        {
        case FetcherState::kFetchTileSleep:
            fetcher_state_ = FetcherState::kFetchTile;
            break;
        case FetcherState::kFetchTile:
        {
            const word tile_map_offset = (lcd_.GetLCDControl() & 0x8) ? 0x1C00 : 0x1800;
            const word tile_address = tile_map_offset + (fetch_y_ / 8 * 32) + fetch_x_;
            tile_number_ = vram_.at(tile_address);
            fetcher_state_ = FetcherState::kFetchDataLowSleep;
            break;
        }
        break;
        case FetcherState::kFetchDataLowSleep:
            fetcher_state_ = FetcherState::kFetchDataLow;
            break;
        case FetcherState::kFetchDataLow:
        {
            const bool tile_data_select = lcd_.GetLCDControl() & 0x10;
            const word tile_base_address = tile_data_select ? 0 : 0x1000;
            int tile_offset = (tile_data_select ? tile_number_ : (signed_byte)(tile_number_));
            tile_offset *= 16;
            const int total_offset = tile_base_address + tile_offset + ((fetch_y_ % 8) * 2);
            tile_data_low_ = vram_.at(total_offset);
            fetcher_state_ = FetcherState::kFetchDataHighSleep;
        }
        break;
        case FetcherState::kFetchDataHighSleep:
            fetcher_state_ = FetcherState::kFetchDataHigh;
            break;
        case FetcherState::kFetchDataHigh:
        {
            const bool tile_data_select = lcd_.GetLCDControl() & 0x10;
            const word tile_base_address = tile_data_select ? 0 : 0x1000;
            const int tile_offset = (tile_data_select ? tile_number_ : (signed_byte)(tile_number_)) * 16;
            const int total_offset = tile_base_address + tile_offset + ((fetch_y_ % 8) * 2 + 1);
            tile_data_high_ = vram_.at(total_offset);
            fetcher_state_ = FetcherState::kSleep1;

            TryPush();
        }
        break;
        case FetcherState::kSleep1:
            fetcher_state_ = FetcherState::kSleep2;
            break;

        case FetcherState::kSleep2:
            fetcher_state_ = FetcherState::kPush;
            break;
        case FetcherState::kPush:
            TryPush();
            break;
        }
    }

    void PPU::Pipeline::TryPush()
    {
        if (background_fifo_.size() <= 8) {
            for (byte i = 0; i < 8; ++i)
            {
                byte color_bit_0 = !!(tile_data_low_ & (1 << (7 - i)));
                byte color_bit_1 = !!(tile_data_high_ & (1 << (7 - i))) << 1;
                byte color = color_bit_0 | color_bit_1;
                background_fifo_.push({ color, 0, 0, 0 });
            }
            fetch_x_ = (fetch_x_ + 1) & 0x1F;
            fetcher_state_ = FetcherState::kFetchTile;
        }
    }

    void PPU::Pipeline::RenderPixel()
    {
        // Pixels won’t be pushed to the LCD if there is nothing in the background FIFO or the current pixel is pixel 160 or greater.
        if (background_fifo_.size() <= 8 || pixels_pushed_ >= 160 || background_fifo_.empty())
            return;

        Pixel background_pixel = background_fifo_.front();
        background_fifo_.pop();

        // When the background pixel is disabled the pixel color value will be 0, otherwise the color value will be whatever color pixel was popped off the background FIFO.
        if ((lcd_.GetLCDControl() & 1) == 0) {
            background_pixel.color = 0;
        }

        Pixel sprite_pixel;
        if (!sprite_fifo_.empty()) {
            sprite_pixel = sprite_fifo_.front();
            sprite_fifo_.pop();
        }
        else
            sprite_pixel.color = 0;

        /* We render a bg pixel when
         * 1. There is no sprite pixel
         * 2. The sprite pixel is transparent (color 0)
         * 3. The background pixel is not transparent and the sprite pixel gives the background pixel priority (bit 7 of sprite attributes is set) */
        if (sprite_pixel.color == 0 || (sprite_pixel.background_priority && background_pixel.color != 0))
            lcd_.RenderPixel(pixels_pushed_, background_pixel.color, false, 0);
        else
            lcd_.RenderPixel(pixels_pushed_, sprite_pixel.color, true, sprite_pixel.palette);

        ++pixels_pushed_;
    }
} // namespace gandalf