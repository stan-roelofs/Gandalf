#ifndef __GANDALF_SOUND_CHANNEL_H
#define __GANDALF_SOUND_CHANNEL_H

#include <gandalf/types.h>

namespace gandalf
{
    class SoundChannel
    {
    public:
        SoundChannel();
        virtual ~SoundChannel();

        /**
         * @param index which register to get (0-4)
         * @return value of register
         */
        virtual byte GetRegister(int index) const = 0;

        /**
         * @param index which register to set (0-4)
         * @param value the new value of the register
         */
        virtual void SetRegister(int index, byte value) = 0;

        bool GetEnabled() const;

        /// @returns the current sample
        virtual byte Tick() = 0;

    protected:
        bool channel_enabled_;
    };
} // namespace gandalf

#endif