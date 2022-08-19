#ifndef __GANDALF_SOUND_FREQUENCY_SWEEP_UNIT_H__
#define __GANDALF_SOUND_FREQUENCY_SWEEP_UNIT_H__

#include "frame_sequencer.h"
#include <gandalf/types.h>

namespace gandalf
{
    /**
     * A frequency sweep unit is used to change the frequency of a channel
     * over time.
     */
    class FrequencySweepUnit : public FrameSequencer::Listener
    {
    public:
        FrequencySweepUnit(byte& frequency_low, byte& frequency_high, bool& sound_channel_enabled);
        virtual ~FrequencySweepUnit();

        virtual void OnFrameSequencerStep() override;
        virtual std::array<bool, 8> GetSteps() const override;

        void Trigger();

        void SetFrequency(word frequency);
        word GetFrequency() const;

        void SetShift(byte shift);
        byte GetShift() const;
        void SetNegate(bool negate);
        bool GetNegate() const;
        void SetPeriod(byte period);
        byte GetPeriod() const;
    private:
        void ReloadTimer();
        word FrequencyCalculation();

        bool& sound_channel_enabled_;
        byte& frequency_low_;
        byte& frequency_high_;

        bool enabled_;
        word frequency_shadow_register_;
        byte period_;
        byte shift_;
        bool negate_;
        byte timer_;
    };
} // namespace gandalf

#endif