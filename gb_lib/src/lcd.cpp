#include <gandalf/lcd.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/util.h>

namespace {
    constexpr gandalf::LCD::BGR565 kColorsDMG[4] = { 0xE7DA, 0x8E0E, 0x334A, 0x08C4 };
}

namespace gandalf
{
    LCD::LCD() : Bus::AddressHandler("LCD"), lcdc_(0), ly_(0), lyc_(0), stat_(0), scy_(0), scx_(0), wy_(0), wx_(0), bgp_(0), obp0_(0), obp1_(0)
    {
        video_buffer_.fill((byte)std::rand());
    }

    LCD::~LCD() = default;

    byte LCD::Read(word address) const
    {
        assert(address == kLCDC || address == kSTAT || address == kSCY ||
            address == kSCX || address == kLY || address == kLYC ||
            address == kBGP || address == kOBP0 || address == kOBP1 || address == kWY ||
            address == kWX);

        switch (address)
        {
        case kLCDC:
            return lcdc_;
        case kSTAT:
            return stat_;
        case kSCY:
            return scy_;
        case kSCX:
            return scx_;
        case kLY:
            return ly_;
        case kLYC:
            return lyc_;
        case kWY:
            return wy_;
        case kWX:
            return wx_;
        case kBGP:
            return bgp_;
        case kOBP0:
            return obp0_;
        case kOBP1:
            return obp1_;;
        default:
            return 0xFF;
        }
    }

    void LCD::Write(word address, byte value)
    {
        assert(address == kLCDC || address == kSTAT || address == kSCY ||
            address == kSCX || address == kLY || address == kLYC ||
            address == kBGP || address == kOBP0 || address == kOBP1 || address == kWY ||
            address == kWX);

        switch (address)
        {
        case kLCDC:
            lcdc_ = value;
            break;
        case kSTAT:
            stat_ = value;
            break;
        case kSCY:
            scy_ = value;
            break;
        case kSCX:
            scx_ = value;
            break;
        case kLY:
            ly_ = value;
            break;
        case kLYC:
            lyc_ = value;
            break;
        case kWY:
            wy_ = value;
            break;
        case kWX:
            wx_ = value;
            break;
        case kBGP:
            bgp_ = value;
            break;
        case kOBP0:
            obp0_ = value;
            break;
        case kOBP1:
            obp1_ = value;
            break;
        }
    }

    std::set<word> LCD::GetAddresses() const
    {
        return { kLCDC, kSTAT, kSCY, kSCX, kLY, kLYC, kWY, kWX, kBGP, kOBP0, kOBP1 };
    }

    void LCD::RenderPixel(byte x, byte color_index, bool is_sprite, byte palette_index)
    {
        byte palette = 0;
        if (is_sprite)
            palette = palette_index == 0 ? obp0_ : obp1_;
        else
            palette = bgp_;

        byte color = palette >> (2 * (color_index)) & 0x3;
        video_buffer_[kScreenWidth * ly_ + x] = kColorsDMG[color];
    }
}