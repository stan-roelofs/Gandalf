#include "frequency_sweep_unit.h"

#include <gandalf/constants.h>

namespace
{
    constexpr int kDivider = gandalf::kCPUFrequency / 128;
}

namespace gandalf
{
    FrequencySweepUnit::FrequencySweepUnit(byte& frequency_low, byte& frequency_high, bool& sound_channel_enabled) :
        sound_channel_enabled_(sound_channel_enabled),
        frequency_low_(frequency_low),
        frequency_high_(frequency_high),
        enabled_(false),
        frequency_shadow_register_(0),
        period_(0),
        shift_(0),
        negate_(false),
        timer_(0)
    {

    }

    FrequencySweepUnit::~FrequencySweepUnit() = default;

    void FrequencySweepUnit::Trigger()
    {
        // Square 1's frequency is copied to the shadow register.
        frequency_shadow_register_ = GetFrequency();

        // The sweep timer is reloaded.
        ReloadTimer();

        // The internal enabled flag is set if either the sweep period or shift are non-zero, cleared otherwise.
        enabled_ = shift_ != 0 || period_ != 0;

        // If the sweep shift is non-zero, frequency calculation and the overflow check are performed immediately.
        if (shift_ != 0)
            SetFrequency(FrequencyCalculation());
    }

    void FrequencySweepUnit::SetFrequency(word frequency)
    {
        frequency_low_ = frequency & 0xFF;
        frequency_high_ = (frequency >> 8) & 0b111;
    }

    word FrequencySweepUnit::GetFrequency() const
    {
        return (2048 - (frequency_low_ | (frequency_high_ << 8))) * 4;
    }

    void FrequencySweepUnit::SetShift(byte shift)
    {
        shift_ = shift;
    }

    byte FrequencySweepUnit::GetShift() const
    {
        return shift_;
    }

    void FrequencySweepUnit::SetNegate(bool negate)
    {
        negate_ = negate;
    }

    bool FrequencySweepUnit::GetNegate() const
    {
        return negate_;
    }

    void FrequencySweepUnit::SetPeriod(byte period)
    {
        period_ = period;
    }

    byte FrequencySweepUnit::GetPeriod() const
    {
        return period_;
    }

    void FrequencySweepUnit::ReloadTimer()
    {
        timer_ = period_;
        if (timer_ == 0)
            timer_ = 8;
    }

    word FrequencySweepUnit::FrequencyCalculation()
    {
        /* Frequency calculation consists of taking the value in the frequency shadow register, shifting it right by sweep shift,
        optionally negating the value, and summing this with the frequency shadow register to produce a new frequency. */
        word result = frequency_shadow_register_ >> shift_;
        if (negate_)
            result = -result;

        result = frequency_shadow_register_ + result;

        if (result > 2047) {
            sound_channel_enabled_ = false;
            enabled_ = false;
        }

        return result;
    }


    void FrequencySweepUnit::OnFrameSequencerStep()
    {
        /*
        The sweep timer is clocked at 128 Hz by the frame sequencer. When it generates a clock and the sweep's internal enabled flag is set and the sweep period is not zero,
        a new frequency is calculated and the overflow check is performed. If the new frequency is 2047 or less and the sweep shift is not zero,
        this new frequency is written back to the shadow frequency and square 1's frequency in NR13 and NR14,
        then frequency calculation and overflow check are run AGAIN immediately using this new value, but this second new frequency is not written back.*/
        if (!enabled_ || period_ == 0)
            return;

        timer_--;
        if (timer_ == 0)
        {
            ReloadTimer();
            const word freq = FrequencyCalculation();
            if (freq <= 2047 && shift_ != 0)
            {
                frequency_shadow_register_ = freq;
                SetFrequency(freq);

                FrequencyCalculation();
            }
        }
    }

    std::array<bool, 8> FrequencySweepUnit::GetSteps() const
    {
        return { false, false, true, false, false, false, true, false };
    }
} // namespace gandalf