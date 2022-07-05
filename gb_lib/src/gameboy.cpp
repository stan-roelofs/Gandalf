#include <gandalf/gameboy.h>

namespace gandalf {
    Gameboy::Gameboy() : timer_(bus_), cpu_(*this, bus_)
    {
        bus_.Register(&cpu_);
        bus_.Register(&cartridge_);
        bus_.Register(&timer_);
    }


    Gameboy::~Gameboy()
    {
        bus_.Unregister(&cpu_);
        bus_.Unregister(&cartridge_);
        bus_.Unregister(&timer_);

    }

    bool Gameboy::Load(const Cartridge::ROM& rom)
    {
        return cartridge_.Load(rom);
    }

    void Gameboy::LoadBootROM(const BootROM& boot_rom)
    {
        boot_rom_handler_ = std::make_unique<BootROMHandler>(boot_rom);
        bus_.Register(boot_rom_handler_.get());
    }

    // TODO this is all a mess
    void Gameboy::Run()
    {
        if (boot_rom_handler_ && boot_rom_handler_->Done()) {
            bus_.Unregister(boot_rom_handler_.get());
            bus_.Register(&cartridge_);
            boot_rom_handler_.release();
        }

        cpu_.Tick();
    }


} // namespace gandalf