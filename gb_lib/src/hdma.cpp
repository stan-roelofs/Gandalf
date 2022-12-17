#include <gandalf/hdma.h>

#include <cassert>

#include <gandalf/constants.h>

namespace
{
    const gandalf::byte kCyclesPerByte = 2;
}

namespace gandalf
{
    HDMA::HDMA(GameboyMode mode, Bus& bus, const LCD& lcd) : Bus::AddressHandler("HDMA"),
        bus_(bus),
        lcd_(lcd),
        hdma1_(0),
        hdma2_(0),
        hdma3_(0),
        hdma4_(0),
        remaining_length_(0),
        hblank_(false),
        remaining_bytes_hblank_(0),
        current_byte_(0),
        state_(State::kIdle),
        source_(0),
        destination_(0),
        mode_(mode)
    {
    }

    HDMA::~HDMA() = default;

    void HDMA::Tick()
    {
        switch (state_) {
        case State::kIdle:
        case State::kTerminated:
            break;
        case State::kRead:
            current_byte_ = bus_.Read(source_);
            ++source_;
            state_ = State::kWrite;
            break;
        case State::kWrite:
            assert(remaining_length_ > 0);
            bus_.Write(destination_, current_byte_);
            ++destination_;
            --remaining_length_;

            hdma1_ = source_ >> 8;
            hdma2_ = source_ & 0xFF;
            hdma3_ = destination_ >> 8;
            hdma4_ = destination_ & 0xFF;

            if (hblank_)
            {
                --remaining_bytes_hblank_;
                if (remaining_bytes_hblank_ == 0 && remaining_length_ > 0)
                {
                    state_ = State::kWaitNotHBlank;
                    break;
                }
            }
                        
            if (remaining_length_ == 0)
                state_ = State::kIdle;
            else
                state_ = State::kRead;
            break;
        case State::kWaitHBlank:
            if (lcd_.GetMode() == LCD::Mode::HBlank) {
                state_ = State::kRead;
                remaining_bytes_hblank_ = 0x10;
            }
            break;
        case State::kWaitNotHBlank:
            if (lcd_.GetMode() != LCD::Mode::HBlank)
                state_ = State::kWaitHBlank;
            break;
        }
    }

    byte HDMA::Read(word address) const
    {
        assert(address == kHDMA1 || address == kHDMA2 || address == kHDMA3 || address == kHDMA4 || address == kHDMA5);
            
        if (mode_ != GameboyMode::CGB)
            return 0xFF;

        switch (address)
        {
        case kHDMA1:
        case kHDMA2:
        case kHDMA3:
        case kHDMA4:
            return 0xFF;
            /*
            Reading from Register FF55 returns the remaining length (divided by 10h, minus 1), a value of 0FFh indicates that the transfer has completed.
            It is also possible to terminate an active HBlank transfer by writing zero to Bit 7 of FF55.
            In that case reading from FF55 will return how many $10 “blocks” remained (minus 1) in the lower 7 bits, but Bit 7 will be read as “1”.
            Stopping the transfer doesn’t set HDMA1-4 to $FF. */
        case kHDMA5:
        {
            if (state_ == State::kIdle)
                return 0xFF;

            if (state_ == State::kTerminated)
                return static_cast<byte>(((remaining_length_ / 0x10) - 1) | 0x80);

            return static_cast<byte>((remaining_length_ / 0x10) - 1);
        }
        }

        return 0xFF;
    }

    void HDMA::Write(word address, byte value)
    {
        assert(address == kHDMA1 || address == kHDMA2 || address == kHDMA3 || address == kHDMA4 || address == kHDMA5);

        if (mode_ != GameboyMode::CGB)
            return;

        switch (address)
        {
        case kHDMA1:
            hdma1_ = value;
            break;
        case kHDMA2:
            hdma2_ = value;
            break;
        case kHDMA3:
            hdma3_ = value;
            break;
        case kHDMA4:
            hdma4_ = value;
            break;
        case kHDMA5:
        {
            if (state_ == State::kIdle || state_ == State::kTerminated || (hblank_ && (value & 0x80) != 0))
                StartTransfer(value);
            else if (hblank_)
                state_ = State::kTerminated;
        }
        break;
        }
    }

    void HDMA::StartTransfer(byte value)
    {
        // Start new transfer
        remaining_length_ = ((value & 0x7F) + 1) * 0x10;
        hblank_ = (value & 0x80) != 0;
        source_ = ((hdma1_ << 8) | hdma2_) & 0xFFF0; // The four lower bits of this address will be ignored and treated as 0.
        destination_ = 0x8000 + (((hdma3_ << 8) | hdma4_) & 0x1FF0); // Only bits 12-4 are respected; others are ignored

        // TODO can this overflow? Should we cap the length?

        if (hblank_)
            state_ = State::kWaitHBlank;
        else
            state_ = State::kRead;
    }

    word HDMA::GetRemainingGDMACycles() const
    {
        if (hblank_ || remaining_length_ == 0)
            return 0;

        return remaining_length_ * kCyclesPerByte;
    }

    std::set<word> HDMA::GetAddresses() const
    {
        return { kHDMA1, kHDMA2, kHDMA3, kHDMA4, kHDMA5 };
    }
}