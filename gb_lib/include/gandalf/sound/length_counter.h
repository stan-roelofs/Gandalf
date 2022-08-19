#ifndef __GANDALF_SOUND_LENGTH_COUNTER_H
#define __GANDALF_SOUND_LENGTH_COUNTER_H

#include "frame_sequencer.h"
#include <gandalf/types.h>

namespace gandalf
{
    /**
     * A length counter disables a channel when it decrements to zero
     */
    class LengthCounter : public FrameSequencer::Listener
    {
    public:
        LengthCounter(int full_length, bool& channel_enabled);
        virtual ~LengthCounter();

        void OnFrameSequencerStep() override;
        std::array<bool, 8> GetSteps() const override;

        void Trigger();
        void SetLength(byte length);
        void SetEnabled(bool enabled);
        bool GetEnabled() const;

    private:
        bool counter_enabled_;
        bool& channel_enabled_;
        int remaining_length_;
        int full_length_;
    };
}

#endif