#include <gandalf/gameboy.h>

namespace gandalf {
    Gameboy::Gameboy(const BootROM& boot_rom, const ROM& rom) : io_(bus_), cpu_(io_, bus_), executed_boot_rom_(false)
    {
        LoadROM(rom);
        LoadBootROM(boot_rom);
        bus_.Register(&cpu_);
        bus_.Register(&wram_);
        bus_.Register(&hram_);
    }

    Gameboy::~Gameboy()
    {
        bus_.Unregister(&cpu_);
        bus_.Unregister(&wram_);
        bus_.Unregister(&hram_);
        if (cartridge_)
            bus_.Unregister(cartridge_.get());
    }

    void Gameboy::LoadROM(const ROM& rom)
    {
        {
            std::unique_ptr<Cartridge> cartridge = std::make_unique<Cartridge>();
            if (!cartridge->Load(rom))
                return;
            cartridge_ = std::move(cartridge);
        }

        bus_.Register(cartridge_.get());
        std::shared_ptr<const Cartridge::Header> header = cartridge_->GetHeader();
        const auto cgb_flag = header->GetCGBFlag();
        const auto mode = cgb_flag == Cartridge::CGBFunctionality::kNotSupported ? GameboyMode::DMG : GameboyMode::CGB;
        cpu_.SetGameboyMode(mode);
        wram_.SetGameboyMode(mode);
        io_.SetGameboyMode(mode);
    }

    void Gameboy::LoadBootROM(const BootROM& boot_rom)
    {
        boot_rom_handler_ = std::make_unique<BootROMHandler>(boot_rom);
        bus_.Register(boot_rom_handler_.get());
    }

    bool Gameboy::Ready() const
    {
        return cartridge_ && (boot_rom_handler_ || executed_boot_rom_);
    }

    // TODO this is all a mess
    void Gameboy::Run()
    {
        if (!Ready())
            return;

        if (boot_rom_handler_ && boot_rom_handler_->Done()) {
            bus_.Unregister(boot_rom_handler_.get());
            bus_.Register(cartridge_.get());
            boot_rom_handler_.release();
            executed_boot_rom_ = true;
        }

        cpu_.Tick();
    }


} // namespace gandalf