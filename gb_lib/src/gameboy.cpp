#include <gandalf/gameboy.h>

namespace gandalf {
    Gameboy::Gameboy(const ROM& boot_rom, const ROM& rom, std::shared_ptr<APU::OutputHandler> audio_handler): mode_(GameboyMode::DMG)
    {
        LoadROM(rom);
        LoadBootROM(boot_rom);

        if (boot_rom.size() > 0x100)
            mode_ = GameboyMode::CGB;

        io_ = std::make_unique<IO>(mode_, bus_, audio_handler);
        cpu_ = std::make_unique<CPU>(mode_, *io_, bus_);
        wram_ = std::make_unique<WRAM>(mode_);
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

    void Gameboy::LoadBootROM(const ROM& boot_rom)
    {
        if (boot_rom.size() < 0x100 || !cartridge_)
            return;

        boot_rom_handler_ = std::make_unique<BootROMHandler>(*this, boot_rom);
        bus_.Register(*boot_rom_handler_);
    }

    void Gameboy::OnBootROMFinished()
    {
        bus_.Unregister(*boot_rom_handler_);
        bus_.Register(*cartridge_);
        byte mode = boot_rom_handler_->Read(kKEY0);
        switch (mode)
        {
        case 0x0: mode_ = GameboyMode::DMG; break;
        case 0x4: mode_ = GameboyMode::DMGCompatibility; break;
        default: mode_ = GameboyMode::CGB; break;
        }

        io_->SetMode(mode_);
        cpu_->SetMode(mode_);
        wram_->SetMode(mode_);
    }

    bool Gameboy::Ready() const
    {
        return cartridge_ != nullptr;
    }

    void Gameboy::Run()
    {
        if (!Ready())
            return;

        cpu_->Tick();
    }


} // namespace gandalf