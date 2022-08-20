#ifndef __GANDALF_SOUND_WAVE_CHANNEL_H
#define __GANDALF_SOUND_WAVE_CHANNEL_H

#include <gandalf/sound/sound_channel.h> 
#include "length_counter.h"

namespace gandalf
{
    class WaveChannel : public SoundChannel
    {
    public:
        WaveChannel(FrameSequencer& frame_sequencer, const std::array<byte, 0x20>& wave_ram);
        ~WaveChannel();

        byte GetRegister(int index) const override;
        void SetRegister(int index, byte value) override;

        gandalf::byte Tick() override;

    private:
        void Trigger();
        word GetFrequency() const;

        const std::array<byte, 0x20>& wave_ram_;

        std::shared_ptr<LengthCounter> length_counter_;

        bool dac_enabled_;
        byte volume_shift_; // The number of bits we should shift the wave channel's output by
        byte volume_code_; // Bits 5-6 of NR32
        std::uint32_t timer_;
        word frequency_register_; // Bits 0-7 of the frequency
        byte position_counter_; // The position in the wave ram table
        byte sample_buffer_; // The sample that is currently loaded
    };
} // namespace gandalf

#endif