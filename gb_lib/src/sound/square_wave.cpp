#include <gandalf/sound/square_wave.h>

#include <cassert>

namespace
{
    gandalf::byte kDutyCyclePatterns[4] = { 0b00000001, 0b10000001, 0b10000111, 0b01111110 };
}

namespace gandalf
{
    SquareWave::SquareWave() : SoundChannel(),
        pattern_duty_(0),
        duty_counter_(0),
        frequency_low_(0),
        frequency_high_(0),
        //frequency_sweep_unit_(nr0_, nr3_, nr4_, channel_enabled_), 
        length_counter_(64, channel_enabled_), timer_(0),
        last_output_(0)
    {
    }

    SquareWave::~SquareWave() = default;

    byte SquareWave::GetRegister(int index) const
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
            return 0;
        case 1:
            return (pattern_duty_ << 6) | 0x3F;
        case 2:
            return 0;
        case 3:
            return 0xFF;
        case 4:
        {
            byte result = 0xBF;
            if (length_counter_.GetEnabled())
                result |= 1 << 6;
            return result;
        }
        }

        return 0;
    }

    void SquareWave::SetRegister(int index, byte value)
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
            break;
        case 1:
            pattern_duty_ = value >> 6;
            length_counter_.SetLength(value & 0x3F);
            break;
        case 2:
            break;
        case 3:
            frequency_low_ = value;
            break;
        case 4:
            if (value & 0x80)
                Trigger();

            length_counter_.SetEnabled((value & 0x40) != 0);
            frequency_high_ = value & 0b111;
            break;
        }
    }

    void SquareWave::Trigger()
    {
        channel_enabled_ = true;
        // frequency_sweep_unit_.Trigger();

        timer_ = GetFrequency() * 4;
    }

    word SquareWave::GetFrequency()
    {
        return 2048 - (((frequency_high_) << 8) | frequency_low_);
    }

    gandalf::byte SquareWave::Tick()
    {
        //volume_envelope_.Tick();
        //length_counter_.Tick();

        --timer_;
        if (timer_ == 0)
        {
            timer_ = GetFrequency() * 4;
            last_output_ = (kDutyCyclePatterns[pattern_duty_] & (1 << duty_counter_)) ? 1 : 0;
            duty_counter_ = (duty_counter_ + 1) % 8;
        }

        byte volume = 15; // TODO volume

        return channel_enabled_ ? last_output_ * volume : 0;
    }
} // namespace gandalf