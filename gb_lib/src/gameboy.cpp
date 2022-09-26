#include <gandalf/gameboy.h>

namespace gandalf {
    Gameboy::Gameboy(const BootROM& boot_rom, const ROM& rom) : executed_boot_rom_(false)
    {
        LoadROM(rom);
        LoadBootROM(boot_rom);

        GameboyMode mode = GameboyMode::DMG;
        if (cartridge_)
        {
            std::shared_ptr<const Cartridge::Header> header = cartridge_->GetHeader();
            const auto cgb_flag = header->GetCGBFlag();
            mode = cgb_flag == Cartridge::CGBFunctionality::kNotSupported ? GameboyMode::DMG : GameboyMode::CGB;
        }

        io_ = std::make_unique<IO>(mode, bus_);
        cpu_ = std::make_unique<CPU>(mode, *io_, bus_);
        wram_ = std::make_unique<WRAM>(mode);
        hram_ = std::make_unique<HRAM>();
        bus_.Register(*cpu_);
        bus_.Register(*wram_);
        bus_.Register(*hram_);
    }

    Gameboy::~Gameboy()
    {
        bus_.Unregister(*cpu_);
        bus_.Unregister(*wram_);
        bus_.Unregister(*hram_);
        if (cartridge_)
            bus_.Unregister(*cartridge_);
    }

    void Gameboy::LoadROM(const ROM& rom)
    {
        {
            std::unique_ptr<Cartridge> cartridge = std::make_unique<Cartridge>();
            if (!cartridge->Load(rom))
                return;
            cartridge_ = std::move(cartridge);
        }

        bus_.Register(*cartridge_);
    }

    void Gameboy::LoadBootROM(const BootROM& boot_rom)
    {
        if (boot_rom.size() < 0x100 || !cartridge_)
            return;

        boot_rom_handler_ = std::make_unique<BootROMHandler>(boot_rom, *cartridge_, bus_);
    }

    bool Gameboy::Ready() const
    {
        return cartridge_ != nullptr;
    }

    // TODO this is all a mess
    void Gameboy::Run()
    {
        if (!Ready())
            return;

        cpu_->Tick();
    }


} // namespace gandalf