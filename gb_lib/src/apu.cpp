#include <gandalf/apu.h>

namespace gandalf
{
    APU::APU() : Bus::AddressHandler("APU")
    {
    }

    APU::~APU() = default;

    void APU::Write(word address, byte value)
    {
        (void)address;
        (void)value;
    }

    byte APU::Read(word address) const
    {
        (void)address;
        return 0xFF;
    }

    std::set<word> APU::GetAddresses() const
    {
        return std::set<word>();
    }

    void APU::Tick()
    {
         if (output_handler_)
             output_handler_->Play(0, 0);
    }

    void APU::SetOutputHandler(std::shared_ptr<APU::OutputHandler> handler)
    {
        output_handler_ = handler;
    }
} // namespace gandalf