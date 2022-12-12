#include <gandalf/timer.h>

#include <cassert>
#include <stdexcept>

#include <gandalf/constants.h>

namespace {

    constexpr gandalf::byte selected_bit[4] = { 9, 3, 5, 7 };
}

namespace gandalf
{
    // TODO is initial value correct? verify using tests
    Timer::Timer(Bus& bus): Bus::AddressHandler("Timer"), div_(0), tma_(0), tima_(0), tac_(0), bus_(bus), reload_counter_(0), selected_bit_(0), enabled_(false)
    {
    }

    Timer::~Timer() = default;

    void Timer::OnDIVChanged(word old_div)
    {
        if (enabled_ && (old_div & (1 << selected_bit_)) && (!(div_ & (1 << selected_bit_)))) {
            ++tima_;

            if (tima_ == 0)
                reload_counter_ = 8;            
        }
    }

    void Timer::Tick()
    {
        if (reload_counter_ > 0) {
            --reload_counter_;
            if (reload_counter_ <= 4) {
                tima_ = tma_;
                bus_.Write(kIF, bus_.Read(kIF) | kTimerInterruptMask);
            }
        }

        word prev_div = div_;
        ++div_;

        OnDIVChanged(prev_div);
    }

    void Timer::Write(word address, byte value)
    {
        assert(address == kTAC || address == kTIMA || address == kTMA || address == kDIV);

        switch (address)
        {
        case kTAC:
        {
            byte new_selected_bit = selected_bit[value & 0x3];
            bool new_enable = value & (1 << 2);

            /* When writing to TAC, if the previously selected multiplexer input was 1 and the new input is 0, TIMA will increase too. 
             * This doesnt happen when the timer is disabled, but it also happens when disabling the timer (the same effect as writing to DIV).
            */
            if ((new_enable && (div_ & (1 << selected_bit_)) && (!(div_ & (1 << new_selected_bit)))) || (!new_enable && div_ & (1 << selected_bit_)))
            {
                ++tima_;
                if (tima_ == 0) {
                    tima_ = tma_;
                    bus_.Write(kIF, bus_.Read(kIF) | kTimerInterruptMask);
                }
            }

            tac_ = value;
            enabled_ = new_enable;
            selected_bit_ = new_selected_bit;
            break;
        }
        case kTIMA:
            if (reload_counter_ > 4)
                reload_counter_ = 0;
            if (reload_counter_ == 0)
                tima_ = value;
            break;
        case kTMA:
            tma_ = value;
            break;
        case kDIV:
            word old_div = div_;
            div_ = 0;
            OnDIVChanged(old_div);

            break;
        }
    }

    byte Timer::Read(word address) const
    {
        assert(address == kTAC || address == kTIMA || address == kTMA || address == kDIV);

        switch (address)
        {
        case kTAC:
            return tac_;
        case kTIMA:
            return tima_;
        case kTMA:
            return tma_;
        case kDIV:
            return div_ >> 8;
        default:
            return 0xFF;
        }
    }

    std::set<word> Timer::GetAddresses() const
    {
        return { kTAC, kTIMA, kTMA, kDIV };
    }
}