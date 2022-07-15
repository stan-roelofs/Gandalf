#ifndef __GANDALF_DMA_H
#define __GANDALF_DMA_H

#include "bus.h"

namespace gandalf
{
    class DMA : public Bus::AddressHandler
    {
    public:
        DMA(Bus& bus);
        virtual ~DMA();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        bool InProgress() const { return in_progress_; }

    private:
        Bus& bus_;
        byte dma_;

        bool in_progress_;
        word current_byte_;
        word source_address_;
    };
}

#endif