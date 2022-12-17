#include <gandalf/lcd.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/util.h>

namespace {
    constexpr gandalf::LCD::ABGR1555 kColorsDMG[4] = { 0xEBFC, 0xBB11, 0xA9A6, 0x9061 };
}

namespace gandalf
{
    LCD::LCD(GameboyMode mode): Bus::AddressHandler("LCD"),
        lcdc_(0),
        ly_(0),
        lyc_(0),
        stat_(0),
        scy_(0),
        scx_(0),
        wy_(0),
        wx_(0),
        bgp_(0),
        obp0_(0),
        obp1_(0),
        bcps_(0),
        ocps_(0),
        mode_(mode)
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
            return obp1_;
        case kBCPS:
            return bcps_;
        case kOCPS:
            return ocps_;
        case kBCPD:
            return static_cast<byte>(bcpd_[(bcps_ & 0x3F) / 2] & 0xFF);
        case kOCPD:
            return static_cast<byte>(ocpd_[(ocps_ & 0x3F) / 2] & 0xFF);
        default:
            return 0xFF;
        }
    }

    void LCD::Write(word address, byte value)
    {
        assert(address == kLCDC || address == kSTAT || address == kSCY ||
            address == kSCX || address == kLY || address == kLYC ||
            address == kBGP || address == kOBP0 || address == kOBP1 || address == kWY ||
            address == kWX || address == kOCPS || address == kBCPS || address == kOCPD ||
            address == kBCPD);

        switch (address)
        {
        case kLCDC:
            lcdc_ = value;
            break;
        case kSTAT:
            stat_ = (stat_ & 0b11) | (value & 0xFC);
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
        case kBCPS:
            if (mode_ != GameboyMode::DMG)
                bcps_ = value;
            break;
        case kBCPD:
            if (mode_ != GameboyMode::DMG)
            {
                const byte index = bcps_ & 0x3F;
                if (index % 2 == 0)
                    bcpd_[index / 2] = (bcpd_[index / 2] & 0xFF00) | value;
                else
                    bcpd_[index / 2] = (bcpd_[index / 2] & 0x00FF) | (value << 8) | 0x8000;

                if ((bcps_ & 0x80) != 0)
                    bcps_ = 0x80 | ((index + 1) & 0x3F);
            }
            break;
        case kOCPS:
            if (mode_ != GameboyMode::DMG)
                ocps_ = value;
            break;
        case kOCPD:
            if (mode_ != GameboyMode::DMG) {
                const byte index = ocps_ & 0x3F;
                if (index % 2 == 0)
                    ocpd_[index / 2] = (ocpd_[index / 2] & 0xFF00) | value;
                else
                    ocpd_[index / 2] = (ocpd_[index / 2] & 0x00FF) | (value << 8) | 0x8000;

                if ((ocps_ & 0x80) != 0)
                    ocps_ = 0x80 | ((index + 1) & 0x3F);
            }
            break;
        }
    }

    std::set<word> LCD::GetAddresses() const
    {
        return { kLCDC, kSTAT, kSCY, kSCX, kLY, kLYC, kWY, kWX, kBGP, kOBP0, kOBP1, kBCPS, kBCPD, kOCPS, kOCPD };
    }

    LCD::Mode LCD::GetMode() const
    {
        return static_cast<Mode>(stat_ & 0b11);
    }

    void LCD::SetMode(Mode mode)
    {
        stat_ = (stat_ & 0xFC) | (static_cast<gandalf::byte>(mode) & 0b11);
    }

    LCD::ABGR1555 LCD::GetBackgroundColor(byte color_index, byte palette_index) const
    {
        if (mode_ == GameboyMode::DMG)
        {
            if (palette_index > 0)
                throw std::invalid_argument("Palette index out of range");

            byte color = bgp_ >> (2 * (color_index)) & 0x3;
            return kColorsDMG[color];
        }
        else
        {
            if (palette_index > 7)
                throw std::invalid_argument("Palette index out of range");

            return bcpd_[palette_index * 4 + color_index];
        }

        assert(false);
        return 0;
    }

    LCD::ABGR1555 LCD::GetSpriteColor(byte color_index, byte palette_index) const
    {
        if (mode_ == GameboyMode::DMG)
        {
            if (palette_index > 1)
                throw std::invalid_argument("Palette index out of range");

            byte palette = palette_index == 0 ? obp0_ : obp1_;
            return kColorsDMG[palette >> (2 * (color_index)) & 0x3];
        }
        else {
            if (palette_index > 7)
                throw std::invalid_argument("Palette index out of range");

            return ocpd_[palette_index * 4 + color_index];
        }

        assert(false);
        return 0;
    }

    void LCD::RenderPixel(byte x, byte color_index, bool is_sprite, byte palette_index)
    {
        video_buffer_[kScreenWidth * ly_ + x] = is_sprite ? GetSpriteColor(color_index, palette_index) : GetBackgroundColor(color_index, palette_index);
    }
}