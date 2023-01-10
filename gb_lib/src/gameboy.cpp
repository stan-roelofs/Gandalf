#include <gandalf/gameboy.h>

#include <gandalf/exception.h>

#include "bootrom.h"

namespace gandalf {

    constexpr GameboyMode GetPreferredMode(Model model)
    {
        switch (model)
        {
        case Model::DMG0:
        case Model::DMG:
        case Model::MGB:
        case Model::SGB:
        case Model::SGB2:
            return GameboyMode::DMG;
        case Model::CGB0:
        case Model::CGB:
            return GameboyMode::CGB;
        default:
            throw InvalidArgument("Invalid model");
        }
    }

    Gameboy::Gameboy(Model emulated_model):
        mode_(GetPreferredMode(emulated_model)),
        model_(emulated_model),
        io_(mode_, memory_),
        cpu_(mode_, io_, memory_),
        wram_(mode_)
    {
        memory_.Register(cpu_);
        memory_.Register(wram_);
        memory_.Register(hram_);
    }

    Gameboy::~Gameboy()
    {
        memory_.Unregister(cpu_);
        memory_.Unregister(wram_);
        memory_.Unregister(hram_);
        if (cartridge_.Loaded())
            memory_.Unregister(cartridge_);
    }

    bool Gameboy::LoadROM(const ROM& rom)
    {
        if (!cartridge_.Load(rom))
            return false;

        memory_.Register(cartridge_);

        // We need to register the boot ROM after the cartridge, loading the cartridge last would overwrite the boot ROM
        const auto boot_rom_bytes = GetBootROM(model_);
        boot_rom_handler_ = std::make_unique<BootROMHandler>(*this, boot_rom_bytes);
        memory_.Register(*boot_rom_handler_);

        return true;
    }

    void Gameboy::SetAudioHandler(std::shared_ptr<APU::OutputHandler> handler)
    {
        io_.GetAPU().SetAudioHandler(handler);
    }

    void Gameboy::AddVBlankListener(PPU::VBlankListener* listener)
    {
        io_.GetPPU().AddVBlankListener(listener);
    }

    void Gameboy::SetButtonState(Joypad::Button button, bool pressed)
    {
        io_.GetJoypad().SetButtonState(button, pressed);
    }

    void Gameboy::MuteAudioChannel(APU::Channel channel, bool mute)
    {
        io_.GetAPU().MuteChannel(channel, mute);
    }

    void Gameboy::RegisterAddressHandler(Memory::AddressHandler& handler)
    {
        memory_.Register(handler);
    }

    void Gameboy::OnBootROMFinished()
    {
        memory_.Unregister(*boot_rom_handler_);
        assert(cartridge_.Loaded());
        memory_.Register(cartridge_);

        const byte key0 = boot_rom_handler_->Read(kKEY0);
        if (model_ == Model::CGB && key0 == 0x4)
        {
            mode_ = GameboyMode::DMGCompatibility;
            io_.SetMode(mode_);
            cpu_.SetMode(mode_);
            wram_.SetMode(mode_);
        }
    }

    void Gameboy::Run()
    {
        if (!cartridge_.Loaded())
            return;

        cpu_.Tick();
    }


} // namespace gandalf