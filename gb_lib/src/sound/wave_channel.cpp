#include "wave_channel.h"

#include <cassert>

namespace {
    const gandalf::byte kVolumeCodeToShift[4] = { 4, 0, 1, 2 };
}

namespace gandalf
{
    WaveChannel::WaveChannel(FrameSequencer& frame_sequencer, const std::array<byte, 0x20>& wave_ram) : SoundChannel(),
        wave_ram_(wave_ram),
        length_counter_(std::make_shared<LengthCounter>(256, channel_enabled_)),
        dac_enabled_(false),
        volume_code_(0),
        timer_(0),
        frequency_register_(0),
        position_counter_(0),
        sample_buffer_(0)
    {
        frame_sequencer.AddListener(length_counter_);
    }

    WaveChannel::~WaveChannel() = default;

    byte WaveChannel::GetRegister(int index) const
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
            return dac_enabled_ ? 0xFF : 0x7F;
        case 1:
            return 0xFF;
        case 2:
            return (volume_code_ << 5) | 0x9F;
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

    void WaveChannel::SetRegister(int index, byte value)
    {
        assert(index >= 0 && index <= 4);

        switch (index)
        {
        case 0:
            dac_enabled_ = (value & 0x80) != 0;
            channel_enabled_ &= dac_enabled_;
            break;
        case 1:
            length_counter_->SetLength(value);
            break;
        case 2:
            volume_code_ = (value >> 5) & 0b11;
            break;
        case 3:
            frequency_register_ = (frequency_register_ & 0x700) | value;
            break;
        case 4:
            frequency_register_ = (frequency_register_ & 0xFF) | ((value & 0b111) << 8);
            length_counter_->SetEnabled((value & 0x40) != 0);

            if (value & 0x80)
                Trigger();

            break;
        }
    }

    void WaveChannel::Trigger()
    {
        if (dac_enabled_)
            channel_enabled_ = true;

        position_counter_ = 0;
        length_counter_->Trigger();

        timer_ = GetFrequency();
    }

    word WaveChannel::GetFrequency() const
    {
        return (2048 - frequency_register_) * 2;
    }

    gandalf::byte WaveChannel::Tick()
    {
        --timer_;
        if (timer_ == 0) {
            timer_ = GetFrequency();
            // When the timer generates a clock, the position counter is advanced one sample in the wave table, looping back to the beginning when it goes past the end, 
            // then a sample is read into the sample buffer from this NEW position.
            position_counter_ = (position_counter_ + 1) % 32;
            sample_buffer_ = wave_ram_[position_counter_ / 2];
            if (position_counter_ % 2 == 0)
                sample_buffer_ >>= 4;

            sample_buffer_ &= 0x0F;
        }

        return channel_enabled_ ? sample_buffer_ >> kVolumeCodeToShift[volume_code_] : 0;
    }
} // namespace gandalf