#include <gandalf/gameboy.h>

namespace gandalf {
    Gameboy::Gameboy() : bus_(std::make_unique<Bus>()), io_(std::make_unique<IO>(*bus_)), cpu_(std::make_unique<CPU>(*io_, *bus_)), wram_(std::make_unique<WRAM>()), cartridge_(std::make_unique<Cartridge>())
    {
        bus_->Register(cpu_.get());
        bus_->Register(wram_.get());
        bus_->Register(cartridge_.get());
    }

    Gameboy::~Gameboy()
    {
        bus_->Unregister(cpu_.get());
        bus_->Unregister(wram_.get());
        bus_->Unregister(cartridge_.get());
    }

    bool Gameboy::Load(const Cartridge::ROM& rom)
    {
        return cartridge_->Load(rom);
    }

    void Gameboy::LoadBootROM(const BootROM& boot_rom)
    {
        boot_rom_handler_ = std::make_unique<BootROMHandler>(boot_rom);
        bus_->Register(boot_rom_handler_.get());
    }

    // TODO this is all a mess
    void Gameboy::Run()
    {
        if (boot_rom_handler_ && boot_rom_handler_->Done()) {
            bus_->Unregister(boot_rom_handler_.get());
            bus_->Register(cartridge_.get());
            boot_rom_handler_.release();
        }

        cpu_->Tick();
    }


} // namespace gandalf