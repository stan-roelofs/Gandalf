#include "noise_channel.h"

#include <cassert>

namespace
{
    gandalf::byte kDivisorCodeToDivisor[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };
}

namespace gandalf
{
    NoiseChannel::NoiseChannel(FrameSequencer& frame_sequencer) : SoundChannel(),
        length_counter_(std::make_shared<LengthCounter>((byte)64, channel_enabled_)),
        volume_envelope_(std::make_shared<VolumeEnvelope>()),
        last_output_(0),
        timer_(0),
        lfsr_(0),
        clock_shift_(0),
        width_mode_bit_(false),
        divisor_code_(0)
    {
        frame_sequencer.AddListener(length_counter_);
        frame_sequencer.AddListener(volume_envelope_);
    }

    NoiseChannel::~NoiseChannel() = default;

    byte NoiseChannel::GetRegister(int index) const
    {
        assert(index >= 0 && index <= 4);
        switch (index)
        {
        case 0:
            return 0xFF;
        case 1:
            return 0xFF;
        case 2:
        {
            byte result = volume_envelope_->GetPeriod() | (volume_envelope_->GetStartingVolume() << 4);
            if (volume_envelope_->GetAddMode())
                result |= 0x8;
            return result;
        }
        case 3:
        {
            byte result = divisor_code_ | (clock_shift_ << 4);
            if (width_mode_bit_)
                result |= 0x8;
            return result;
        }
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

    void NoiseChannel::SetRegister(int index, byte value)
    {
        assert(index >= 0 && index <= 4);
        switch (index)
        {
        case 0:
            break;
        case 1:
            length_counter_->SetLength(value & 0x3F);
            break;
        case 2:
            volume_envelope_->SetPeriod(value & 0x7);
            volume_envelope_->SetAddMode((value & 0x8) != 0);
            volume_envelope_->SetStartingVolume(value >> 4);
            break;
        case 3:
            clock_shift_ = value >> 4;
            width_mode_bit_ = (value & 0x8) != 0;
            divisor_code_ = value & 0x7;
            break;
        case 4:
            length_counter_->SetEnabled((value & 0x40) != 0);

            if (value & 0x80)
                Trigger();

            break;
        }
    }

    byte NoiseChannel::Tick()
    {
        timer_--;
        if (timer_ == 0)
        {
            timer_ = GetFrequency();

            word shifted = lfsr_ >> 1;
            byte xor_result = (lfsr_ ^ shifted) & 1;
            lfsr_ = shifted | (xor_result << 14);

            if (width_mode_bit_)
                lfsr_ = (lfsr_ & ~0x40) | xor_result << 6;

            last_output_ = (lfsr_ & 1) ^ 1;
        }

        return channel_enabled_ ? last_output_ * volume_envelope_->GetVolume() : 0;
    }

    word NoiseChannel::GetFrequency() const
    {
        return  kDivisorCodeToDivisor[divisor_code_] << clock_shift_;
    }

    void NoiseChannel::Trigger()
    {
        channel_enabled_ = true;
        length_counter_->Trigger();
        volume_envelope_->Trigger();

        timer_ = GetFrequency();
        lfsr_ = 0x7FFF;
    }
} // namespace gandalf