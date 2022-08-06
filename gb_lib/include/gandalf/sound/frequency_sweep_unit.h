#ifndef __GANDALF_SOUND_FREQUENCY_SWEEP_UNIT_H__
#define __GANDALF_SOUND_FREQUENCY_SWEEP_UNIT_H__

#include <gandalf/types.h>

namespace gandalf
{
    class FrequencySweepUnit
    {
    public:
        FrequencySweepUnit(byte& nr10, byte& nr13, byte& nr14, bool& sound_channel_enabled);
        ~FrequencySweepUnit();

        /**
         * During a trigger event, several things occur:
         *   Square 1's frequency is copied to the shadow register.
         *   The sweep timer is reloaded.
         *   The internal enabled flag is set if either the sweep period or shift are non-zero, cleared otherwise.
         *   If the sweep shift is non-zero, frequency calculation and the overflow check are performed immediately.
         */
        void Trigger();
        void Tick();

    private:
        void ReloadTimer();
        byte GetPeriod() const;
        byte GetShift() const;
        bool GetNegate() const;
        byte GetFrequency() const;
        void SetFrequency(word frequency);
        word CalculateFrequency();

        // TODO should these be references or can they be owned by this class? does squarewave need to own these?
        byte& nr10_;
        byte& nr13_;
        byte& nr14_;
        bool& sound_channel_enabled_;

        int frame_sequencer_counter_;
        int timer_;
        bool enabled_;
        word frequency_shadow_register_;
    };
} // namespace gandalf

#endif