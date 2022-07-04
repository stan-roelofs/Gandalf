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

    bool Gameboy::Boot(const std::array<byte, 0x100>& boot_rom, const std::vector<byte>& rom)
    {
        if (!cartridge_.Load(rom))
            return false;

        struct BootROM : Bus::AddressHandler
        {
            BootROM(const std::array<byte, 0x100>& boot_rom) : Bus::AddressHandler("Boot ROM"), should_unregister_(false), boot_rom_(boot_rom)
            {
            }

            void Write(word address, byte value) override
            {
                if (address != kBANK)
                    return;

                if (value != 0)
                    should_unregister_ = true;
            }

            byte Read(word address) const override
            {
                if (address < 0x100)
                    return boot_rom_[address];

                throw std::runtime_error("Invalid read from boot ROM");
            }

            std::set<word> GetAddresses() const override
            {
                std::set<word> addresses;
                for (word i = 0; i < 0x100; ++i)
                    addresses.insert(i);
                addresses.insert(kBANK);
                return addresses;
            }

            bool Done() const { return should_unregister_; }

        private:
            bool should_unregister_;
            const std::array<byte, 0x100>& boot_rom_;
        } boot_rom_handler(boot_rom);

        while (!boot_rom_handler.Done())
            cpu_.Tick();

        // Now unmap the boot ROM and map the cartridge ROM
        bus_.Unregister(&boot_rom_handler);
        bus_.Register(&cartridge_);

        return true;
    }

    void Gameboy::Run()
    {
        //cpu_.Run();
    }


} // namespace gandalf