#ifndef __GANDALF_SOUND_NOISE_CHANNEL_H
#define __GANDALF_SOUND_NOISE_CHANNEL_H

#include "sound_channel.h"

#include "length_counter.h"
#include "volume_envelope.h"

namespace gandalf
{
    class NoiseChannel : public SoundChannel
    {
    public:
        NoiseChannel(FrameSequencer& frame_sequencer);
        ~NoiseChannel();

        byte GetRegister(int index) const override;
        void SetRegister(int index, byte value) override;

        gandalf::byte Tick() override;

    private:
        void Trigger();
        word GetFrequency() const;

        std::shared_ptr<LengthCounter> length_counter_;
        std::shared_ptr<VolumeEnvelope> volume_envelope_;

        byte last_output_;
        word timer_;
        word lfsr_;

        // NR43
        byte clock_shift_; // bits 4-7
        bool width_mode_bit_; // Bit 3
        byte divisor_code_; // Bits 0-2
    };
} // namespace gandalf

#endif