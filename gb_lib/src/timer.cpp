#include <gandalf/timer.h>

#include <stdexcept>

#include <gandalf/constants.h>

namespace {
    bool TimerEnabled(gandalf::byte tac) {
        return tac & (1 << 2);
    }

    bool ShouldIncreaseTimer(gandalf::byte tac, gandalf::word prev_div, gandalf::word div) {
        if (!TimerEnabled(tac))
            return false;

        const gandalf::byte mode = tac & 0x3;
        if (mode == 0)
            return (prev_div & (1 << 9)) && (!(div & (1 << 9)));
        else if (mode == 0x1)
            return (prev_div & (1 << 3)) && (!(div & (1 << 3)));
        else if (mode == 0x2)
            return (prev_div & (1 << 5)) && (!(div & (1 << 5)));
        else
            return (prev_div & (1 << 7)) && (!(div & (1 << 7)));
    }
}

namespace gandalf
{
    // TODO is initial value correct? verify using tests
    Timer::Timer(Bus& bus) : Bus::AddressHandler("Timer"), div_(0xAC00), tma_(0), tima_(0), tac_(0), bus_(bus)
    {
    }

    Timer::~Timer() = default;

    void Timer::Tick()
    {
        // TODO reloading takes some cycles 

        word prev_div = div_;

        div_++;

        if (ShouldIncreaseTimer(tac_, prev_div, div_)) {
            tima_++;

            if (tima_ == 0) {
                tima_ = tma_;

                bus_.Write(kIF, bus_.Read(kIF) | kTimerInterruptMask);
            }
        }
    }

    void Timer::Write(word address, byte value)
    {
        switch (address)
        {
        case kTAC:
            // TODO disabling can cause TIMA increase, we'll ignore this for now and implement it later
            tac_ = value;
            break;
        case kTIMA:
            tima_ = value;
            break;
        case kTMA:
            tma_ = value;
            break;
        case kDIV:
            // TODO writing to DIV can cause TIMA increase, we'll ignore this for now and implement it later
            div_ = 0;
            break;
            //default:
                //throw Exception("Invalid timer address");
        }
    }

    byte Timer::Read(word address) const
    {
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
            return 0xFF; // TODO
            // throw Exception("Invalid timer address");
        }
    }

    std::set<word> Timer::GetAddresses() const
    {
        return { kTAC, kTIMA, kTMA, kDIV };
    }
}