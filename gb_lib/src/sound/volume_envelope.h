#ifndef __GANDALF_SOUND_VOLUME_ENVELOPE_H
#define __GANDALF_SOUND_VOLUME_ENVELOPE_H

#include <gandalf/types.h>
#include <gandalf/sound/frame_sequencer.h> 

namespace gandalf
{
    /**
     * A volume envelope has a volume counter and an internal timer clocked at 64 Hz by the frame sequencer.
     * When the timer generates a clock and the envelope period is not zero, a new volume is calculated by adding or subtracting one from the current volume */
    class VolumeEnvelope : public FrameSequencer::Listener
    {
    public:
        VolumeEnvelope(bool& sound_channel_enabled);
        virtual ~VolumeEnvelope();

        void OnFrameSequencerStep() override;
        std::array<bool, 8> GetSteps() const override;

        void Trigger();
        byte GetVolume() const;

        byte GetStartingVolume() const;
        void SetStartingVolume(byte volume);
        bool GetAddMode() const;
        void SetAddMode(bool add_mode);
        byte GetPeriod() const;
        void SetPeriod(byte period);

    private:
        void CheckDACEnabled();

        bool& sound_channel_enabled_;
        byte counter_;
        bool enabled_;
        byte current_volume_;

        // NR22
        byte starting_volume_; // Bits 4-7
        bool add_mode_; // Bit 3
        byte period_; // Bits 0-2
    };
}

#endif