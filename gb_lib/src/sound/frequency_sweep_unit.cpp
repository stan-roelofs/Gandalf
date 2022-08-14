#include <gandalf/sound/frequency_sweep_unit.h>

#include <gandalf/constants.h>

namespace
{
    constexpr int kDivider = gandalf::kCPUFrequency / 128;
}

namespace gandalf
{
    FrequencySweepUnit::FrequencySweepUnit(byte& nr10, byte& nr13, byte& nr14, bool& sound_channel_enabled) :
        nr10_(nr10),
        nr13_(nr13),
        nr14_(nr14),
        sound_channel_enabled_(sound_channel_enabled),
        frame_sequencer_counter_(0),
        timer_(0),
        enabled_(false),
        frequency_shadow_register_(0)
    {

    }

    FrequencySweepUnit::~FrequencySweepUnit() = default;

    void FrequencySweepUnit::Tick()
    {
        ++frame_sequencer_counter_;
        if (frame_sequencer_counter_ < kDivider)
            return;

        frame_sequencer_counter_ = 0;
        if (!enabled_)
            return;

        --timer_;
        if (timer_ > 0)
            return;

        ReloadTimer();

    }

    void FrequencySweepUnit::Trigger()
    {
        // Square 1's frequency is copied to the shadow register.
        frequency_shadow_register_ = GetFrequency();

        // The sweep timer is reloaded.
        ReloadTimer();

        const byte shift = GetShift();
        // The internal enabled flag is set if either the sweep period or shift are non-zero, cleared otherwise.
        enabled_ = shift != 0 || GetPeriod() != 0;

        // If the sweep shift is non-zero, frequency calculation and the overflow check are performed immediately.
        if (shift != 0)
            SetFrequency(CalculateFrequency());
    }

    void FrequencySweepUnit::ReloadTimer()
    {
        timer_ = GetPeriod();

        // The volume envelope and sweep timers treat a period of 0 as 8.
        if (timer_ == 0)
            timer_ = 8;
    }

    word FrequencySweepUnit::CalculateFrequency()
    {
        /* Frequency calculation consists of taking the value in the frequency shadow register,
        shifting it right by sweep shift, optionally negating the value,
        and summing this with the frequency shadow register to produce a new frequency.

        The overflow check simply calculates the new frequency and if this is greater than 2047, square 1 is disabled
        */
        word new_frequency_ = frequency_shadow_register_ >> GetShift();
        if (GetNegate())
            new_frequency_ = frequency_shadow_register_ - new_frequency_;
        else
            new_frequency_ = frequency_shadow_register_ + new_frequency_;

        if (new_frequency_ > 2047) {
            enabled_ = false;
            sound_channel_enabled_ = false;
        }

        return new_frequency_;
    }

    byte FrequencySweepUnit::GetPeriod() const
    {
        return nr10_ >> 4;
    }

    byte FrequencySweepUnit::GetShift() const
    {
        return nr10_ & 0x7;
    }

    bool FrequencySweepUnit::GetNegate() const
    {
        return (nr10_ & 0x8) != 0;
    }

    byte FrequencySweepUnit::GetFrequency() const
    {
        return ((nr14_ & 0x7) << 8) | nr13_;
    }

    void FrequencySweepUnit::SetFrequency(word frequency)
    {
        nr13_ = frequency & 0xFF;
        nr14_ |= (frequency >> 8) & 0x7;
    }
} // namespace gandalf