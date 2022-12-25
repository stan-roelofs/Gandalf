#ifndef __GANDALF_DMA_H
#define __GANDALF_DMA_H

#include "memory.h"

namespace gandalf
{
    class DMA: public Memory::AddressHandler
    {
    public:
        DMA(Memory& memory);
        virtual ~DMA();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        void Start();
        Memory& memory_;
        byte dma_;

        bool in_progress_;
        word current_byte_read_;
        word current_byte_write_;
        word source_address_;
        byte read_value_;
        int cycle_counter_;
        Memory::Bus bus_;
    };
}

#endif