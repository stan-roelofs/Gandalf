#ifndef __GANDALF_SOUND_SQUARE_WAVE_H
#define __GANDALF_SOUND_SQUARE_WAVE_H

#include <gandalf/sound/sound_channel.h>

#include "frequency_sweep_unit.h"
#include "length_counter.h"

namespace gandalf
{
    class SquareWave : public SoundChannel
    {
    public:
        SquareWave();
        virtual ~SquareWave();

        virtual byte GetRegister(int index) const override;

        virtual void SetRegister(int index, byte value) override;

        virtual gandalf::byte Tick();

    private:
        void Trigger();
        word GetFrequency();

        byte pattern_duty_;
        byte duty_counter_;

        byte frequency_low_;
        byte frequency_high_;

        word timer_;
        byte last_output_;

        LengthCounter length_counter_;
        //FrequencySweepUnit frequency_sweep_unit_;
    };

    class SquareWave1 : public SquareWave
    {

    };

    class SquareWave2 : public SquareWave
    {

    };
} // namespace gandalf

#endif