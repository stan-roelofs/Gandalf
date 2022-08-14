#include <gandalf/sound/volume_envelope.h>

namespace gandalf
{
    VolumeEnvelope::VolumeEnvelope() :
        counter_(0),
        enabled_(false),
        current_volume_(0),
        starting_volume_(0),
        add_mode_(false),
        period_(0)
    {
    }

    VolumeEnvelope::~VolumeEnvelope() = default;

    void VolumeEnvelope::OnFrameSequencerStep()
    {
        /*
        When the timer generates a clock and the envelope period is not zero,
        a new volume is calculated by adding or subtracting (as set by NRx2) one from the current volume.
        If this new volume within the 0 to 15 range, the volume is updated,
        otherwise it is left unchanged and no further automatic increments/decrements are made to the volume until the channel is triggered again.
        */
        if (!enabled_ || period_ == 0)
            return;

        --counter_;
        if (counter_ != 0)
            return;

        counter_ = period_;

        const byte new_volume = current_volume_ + (add_mode_ ? 1 : -1);
        if (new_volume > 0xF) {
            enabled_ = false;
            return;
        }

        current_volume_ = new_volume;
    }

    std::array<bool, 8> VolumeEnvelope::GetSteps() const
    {
        std::array<bool, 8> steps;
        steps.fill(false);
        steps[7] = true;
        return steps;
    }

    void VolumeEnvelope::Trigger()
    {
        enabled_ = true;
        current_volume_ = starting_volume_;
        counter_ = period_;
    }

    byte VolumeEnvelope::GetVolume() const
    {
        return current_volume_;
    }

    byte VolumeEnvelope::GetStartingVolume() const
    {
        return starting_volume_;
    }

    void VolumeEnvelope::SetStartingVolume(byte volume)
    {
        starting_volume_ = volume & 0xF;
    }

    bool VolumeEnvelope::GetAddMode() const
    {
        return add_mode_;
    }

    void VolumeEnvelope::SetAddMode(bool add_mode)
    {
        add_mode_ = add_mode;
    }

    byte VolumeEnvelope::GetPeriod() const
    {
        return period_;
    }

    void VolumeEnvelope::SetPeriod(byte period)
    {
        period_ = period & 0x7;
    }
} // namespace gandalf