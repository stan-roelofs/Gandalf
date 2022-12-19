#include <gandalf/dma.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf
{
    DMA::DMA(Bus& bus): Bus::AddressHandler("DMA"),
        bus_(bus),
        dma_(0),
        in_progress_(false),
        current_byte_read_(0),
        current_byte_write_(0),
        source_address_(0),
        read_value_(0),
        cycle_counter_(0)
    {
    }

    DMA::~DMA() = default;

    /* Requires 160 * 4 + 4 cycles
    * - First cycle we read byte 0
    * - Every cycle after that, we read byte n + 1 and write byte n
    */
    void DMA::Tick()
    {
        if (!in_progress_ || ++cycle_counter_ < 4)
            return;
        cycle_counter_ = 0;

        if (current_byte_read_ > 0) {
            bus_.Write(0xFE00 + current_byte_write_, read_value_, Bus::AccessLevel::kOEMDMA);
            ++current_byte_write_;
        }

        if (current_byte_read_ < 160) {
            read_value_ = bus_.Read(source_address_ + current_byte_read_, Bus::AccessLevel::kOEMDMA);
            ++current_byte_read_;
        }

        if (current_byte_write_ == 160) {
            in_progress_ = false;
            bus_.SetAccessLevel(Bus::AccessLevel::kNormal, 0x0000, 0xFF45);
            bus_.SetAccessLevel(Bus::AccessLevel::kNormal, 0xFF47, 0xFF80);
            bus_.SetAccessLevel(Bus::AccessLevel::kNormal, 0xFFFF, 0xFFFF);
        }
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
        if (address != kDMA)
            return;
        dma_ = value;

        Start(); // TODO startup time?
    }

    void DMA::Start()
    {
        // Unlike normal memory accesses, OAM DMA transfers interpret all accesses in the 0xA000 - 0xFFFF range as external RAM transfers.
        // TODO not sure whether the code below is correct. If I understand correctly DMA will try to read from cartridge RAM that doesn't exist.
        // Whatever happens is probably determined by the cartridge??
        if (dma_ >= 0xF0)
            dma_ -= 0x20;

        cycle_counter_ = 0;
        current_byte_read_ = 0;
        current_byte_write_ = 0;
        read_value_ = 0;
        in_progress_ = true;
        source_address_ = dma_ << 8;

        bus_.SetAccessLevel(Bus::AccessLevel::kOEMDMA, 0x0000, 0xFF45);
        bus_.SetAccessLevel(Bus::AccessLevel::kOEMDMA, 0xFF47, 0xFF7F);
        bus_.SetAccessLevel(Bus::AccessLevel::kOEMDMA, 0xFFFF, 0xFFFF);
    }

    std::set<word> DMA::GetAddresses() const
    {
        return { kDMA };
    }
}