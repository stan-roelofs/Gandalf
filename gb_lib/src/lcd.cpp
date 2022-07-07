#include <gandalf/lcd.h>

#include <gandalf/constants.h>

namespace gandalf
{
    LCD::LCD() : Bus::AddressHandler("LCD"), lcdc_(0), ly_(0), lyc_(0), stat_(0), scy_(0), scx_(0), wy_(0), wx_(0), bgp_(0), obp0_(0), obp1_(0), dma_(0)
    {
    }

    LCD::~LCD() = default;

    void LCD::Tick()
    {
    }

    byte LCD::Read(word address) const
    {
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
        case kDMA:
            return dma_;
        default:
            throw Exception("Invalid LCD address");
        }
    }

    void LCD::Write(word address, byte value)
    {
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
        case kDMA:
            dma_ = value;
            break;
        default:
            throw Exception("Invalid LCD address");
        }
    }

    std::set<word> LCD::GetAddresses() const
    {
        return { kLCDC, kSTAT, kSCY, kSCX, kLY, kLYC, kWY, kWX, kBGP, kOBP0, kOBP1, kDMA };
    }
}