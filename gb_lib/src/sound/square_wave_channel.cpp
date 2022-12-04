#include "square_wave_channel.h"

#include <cassert>

namespace
{
    gandalf::byte kDutyCyclePatterns[4] = { 0b00000001, 0b10000001, 0b10000111, 0b01111110 };
}

namespace gandalf
{
    SquareWaveChannel::SquareWaveChannel(FrameSequencer& frame_sequencer, bool has_frequency_sweep_unit) : SoundChannel(),
        pattern_duty_(0),
        duty_counter_(0),
        frequency_low_(0),
        frequency_high_(0),
        timer_(0),
        last_output_(0),
        length_counter_(std::make_shared<LengthCounter>((byte)64, channel_enabled_)),
        volume_envelope_(std::make_shared<VolumeEnvelope>(channel_enabled_))
    {
        frame_sequencer.AddListener(length_counter_);
        frame_sequencer.AddListener(volume_envelope_);

        if (has_frequency_sweep_unit)
        {
            frequency_sweep_unit_ = std::make_shared<FrequencySweepUnit>(frequency_low_, frequency_high_, channel_enabled_);
            frame_sequencer.AddListener(frequency_sweep_unit_);
        }
    }

    SquareWaveChannel::~SquareWaveChannel() = default;

    byte SquareWaveChannel::GetRegister(int index) const
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
        {
            if (!frequency_sweep_unit_)
                return 0xFF;
            byte result = frequency_sweep_unit_->GetShift();
            if (frequency_sweep_unit_->GetNegate())
                result |= 0x8;
            result |= (frequency_sweep_unit_->GetPeriod() << 4);
            return result;
        }
        case 1:
            return (pattern_duty_ << 6) | 0x3F;
        case 2:
        {
            byte result = volume_envelope_->GetPeriod();
            if (volume_envelope_->GetAddMode())
                result |= 0x8;
            result |= (volume_envelope_->GetStartingVolume() << 4);
            return result;
        }
        case 3:
            return 0xFF;
        case 4:
        {
            byte result = 0xBF;
            if (length_counter_->GetEnabled())
                result |= 1 << 6;
            return result;
        }
        }

        return 0;
    }

    void SquareWaveChannel::SetRegister(int index, byte value)
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
            if (frequency_sweep_unit_)
            {
                frequency_sweep_unit_->SetShift(value & 0x7);
                frequency_sweep_unit_->SetNegate((value & 0x8) != 0);
                frequency_sweep_unit_->SetPeriod((value >> 4) & 0x7);
            }
            break;
        case 1:
            pattern_duty_ = value >> 6;
            length_counter_->SetLength(value & 0x3F);
            break;
        case 2:
            volume_envelope_->SetPeriod(value & 0x7);
            volume_envelope_->SetAddMode((value & 0x8) != 0);
            volume_envelope_->SetStartingVolume(value >> 4);
            break;
        case 3:
            frequency_low_ = value;
            break;
        case 4:
            length_counter_->SetEnabled((value & 0x40) != 0);
            frequency_high_ = value & 0b111;

            if (value & 0x80)
                Trigger();
            break;
        }
    }

    void SquareWaveChannel::Trigger()
    {
        channel_enabled_ = true;
        length_counter_->Trigger();
        volume_envelope_->Trigger(); // Note: this may disable the channel if the DAC is off

        if (frequency_sweep_unit_)
            frequency_sweep_unit_->Trigger();

        timer_ = GetFrequency();
    }

    word SquareWaveChannel::GetFrequency() const
    {
        return (2048 - (((frequency_high_) << 8) | frequency_low_)) * 4;
    }

    gandalf::byte SquareWaveChannel::Tick()
    {
        --timer_;
        if (timer_ == 0) {
            timer_ = GetFrequency();

            last_output_ = (kDutyCyclePatterns[pattern_duty_] & (1 << duty_counter_)) ? 1 : 0;
            duty_counter_ = (duty_counter_ + 1) % 8;
        }

        return channel_enabled_ ? last_output_ * volume_envelope_->GetVolume() : 0;
    }
} // namespace gandalf