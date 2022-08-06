#ifndef __GANDALF_SOUND_LENGTH_COUNTER_H
#define __GANDALF_SOUND_LENGTH_COUNTER_H

#include <gandalf/types.h>

namespace gandalf
{
    /**
     * A length counter disables a channel when it decrements to zero
     */
    class LengthCounter
    {
    public:
        LengthCounter(byte full_length, bool& channel_enabled);
        virtual ~LengthCounter();

        void Tick();
        void Trigger();
        void SetLength(byte length);
        void SetEnabled(bool enabled);
        bool GetEnabled() const;

    private:
        bool counter_enabled_;
        bool& channel_enabled_;
        byte remaining_length_;
        byte full_length_;

        word frame_sequencer_counter_;
    };
}

#endif