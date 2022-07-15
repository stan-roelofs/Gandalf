#include <gandalf/dma.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf
{
    DMA::DMA(Bus& bus) : Bus::AddressHandler("DMA"), bus_(bus), in_progress_(false), current_byte_(0), source_address_(0)
    {
    }

    DMA::~DMA() = default;

    void DMA::Tick()
    {
        if (!in_progress_)
            return;

        const byte source_byte = bus_.Read(source_address_ + current_byte_);
        bus_.Write(0xFE00 + current_byte_, source_byte);

        ++current_byte_;

        if (current_byte_ == 0x100)
            in_progress_ = false;
    }

    byte DMA::Read(word address) const
    {
        assert(address == kDMA);
        (void)address;
        return dma_;
    }

    void DMA::Write(word address, byte value)
    {
        assert(address == kDMA);
        (void)address;
        dma_ = value;

        current_byte_ = 0;
        in_progress_ = true;
        source_address_ = dma_ << 8;
    }

    std::set<word> DMA::GetAddresses() const
    {
        return { kDMA };
    }
}