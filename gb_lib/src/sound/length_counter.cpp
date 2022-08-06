#include <gandalf/sound/length_counter.h>

#include <cassert>

#include <gandalf/constants.h>

namespace
{
    constexpr gandalf::word kFrameSequencerTicks = gandalf::kCPUFrequency / 256;
}

namespace gandalf
{
    LengthCounter::LengthCounter(byte full_length, bool& channel_enabled) :
        frame_sequencer_counter_(0),
        counter_enabled_(false),
        channel_enabled_(channel_enabled),
        remaining_length_(0),
        full_length_(full_length)
    {
        assert(full_length == 256 || full_length == 64);
    }

    LengthCounter::~LengthCounter() = default;

    void LengthCounter::Tick()
    {
        if (frame_sequencer_counter_ == 0)
        {
            frame_sequencer_counter_ = kFrameSequencerTicks;
            if (counter_enabled_ && remaining_length_ > 0)
            {
                --remaining_length_;
                if (remaining_length_ == 0)
                    channel_enabled_ = false;
            }
        }

        --frame_sequencer_counter_;
    }

    void LengthCounter::Trigger()
    {
        if (remaining_length_ == 0)
            remaining_length_ = full_length_;
    }

    void LengthCounter::SetLength(byte length)
    {
        remaining_length_ = full_length_ - length;
    }

    void LengthCounter::SetEnabled(bool enabled)
    {
        counter_enabled_ = enabled;
    }

    bool LengthCounter::GetEnabled() const
    {
        return counter_enabled_;
    }
}