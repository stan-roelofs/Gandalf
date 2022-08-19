#include <gandalf/sound/length_counter.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf
{
    LengthCounter::LengthCounter(int full_length, bool& channel_enabled) :
        counter_enabled_(false),
        channel_enabled_(channel_enabled),
        remaining_length_(0),
        full_length_(full_length)
    {
        assert(full_length == 256 || full_length == 64);
    }

    LengthCounter::~LengthCounter() = default;

    void LengthCounter::OnFrameSequencerStep()
    {
        if (!counter_enabled_ || remaining_length_ == 0)
            return;

        --remaining_length_;
        if (remaining_length_ == 0)
            channel_enabled_ = false;
    }

    std::array<bool, 8> LengthCounter::GetSteps() const
    {
        std::array<bool, 8> steps;
        steps.fill(false);
        for (int i = 0; i < 8; i += 2)
            steps[i] = true;
        return steps;
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