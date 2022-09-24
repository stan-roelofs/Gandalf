#ifndef __GANDALF_HDMA_H
#define __GANDALF_HDMA_H

#include "bus.h"

namespace gandalf
{
    class HDMA : public Bus::AddressHandler
    {
    public:
        HDMA(Bus& bus);
        virtual ~HDMA();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        word GetRemainingGDMACycles() const;

    private:
        void StartTransfer(byte value);
        Bus& bus_;

        byte hdma1_;
        byte hdma2_;
        byte hdma3_;
        byte hdma4_;

        word remaining_length_;

        bool hblank_;

        enum class State
        {
            kIdle,
            kRead,
            kWrite,
            kWaitHBlank,
            kTerminated
        };

        byte current_byte_;
        State state_;

        word source_;
        word destination_;
    };
}

#endif