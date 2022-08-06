#include <gandalf/apu.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/sound/square_wave.h>
#include <gandalf/util.h>

namespace {
    constexpr gandalf::word kFrameSequencerTicks = gandalf::kCPUFrequency / 512;
}

namespace gandalf
{
    APU::APU() : Bus::AddressHandler("APU"),
        sound_enabled_(false),
        vin_left_(false),
        vin_right_(false),
        left_volume_(0),
        right_volume_(0)
    {
        wave_ram_.fill((byte)std::rand());
        samples_.fill(0);

        channel_left_enabled_.fill(false);
        channel_right_enabled_.fill(false);

        //todo dac

        //struct DummyChannel : SoundChannel
        //{
        //    DummyChannel() : SoundChannel() {}
        //    byte Tick() override { return 0; }
        //    void SetRegister(int, byte) override {}
        //    byte GetRegister(int) const override { return 0; }
        //};

        //sound_channels_[0] = std::make_unique<DummyChannel>();
        //sound_channels_[1] = std::make_unique<SquareWave2>();
        //sound_channels_[2] = std::make_unique<DummyChannel>();
        //sound_channels_[3] = std::make_unique<DummyChannel>();
    }

    APU::~APU() = default;

    void APU::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0xFF10, 0xFF27) || BETWEEN(address, 0xFF30, 0xFF40));

        if (address <= kNR14)
            square_wave1_.SetRegister(address - kNR10, value);
        else if (address <= kNR24)
            square_wave2_.SetRegister(address - kNR20, value);
        else if (address == kNR50)
        {
            vin_left_ = (value & 0x80) != 0;
            vin_right_ = (value & 0x08) != 0;
            left_volume_ = (value & 0x07);
            right_volume_ = (value & 0x70) >> 4;
        }
        else if (address == kNR51)
        {
            channel_right_enabled_[0] = (value & 0x01) != 0;
            channel_right_enabled_[1] = (value & 0x02) != 0;
            channel_right_enabled_[2] = (value & 0x04) != 0;
            channel_right_enabled_[3] = (value & 0x08) != 0;
            channel_left_enabled_[0] = (value & 0x10) != 0;
            channel_left_enabled_[1] = (value & 0x20) != 0;
            channel_left_enabled_[2] = (value & 0x40) != 0;
            channel_left_enabled_[3] = (value & 0x80) != 0;
        }
        else if (address == kNR52)
        {
            sound_enabled_ = (value & 0x80) != 0;
        }
        else if (address > 0xFF2F)
            wave_ram_[address - 0xFF30] = value;
    }

    byte APU::Read(word address) const
    {
        assert(BETWEEN(address, 0xFF10, 0xFF27) || BETWEEN(address, 0xFF30, 0xFF40));
        if (address <= kNR14)
            return square_wave1_.GetRegister(address - kNR10);
        else if (address <= kNR24)
            return square_wave2_.GetRegister(address - kNR20);
        else if (address == kNR50)
            return 0xFF; //TODO
        else if (address == kNR51)
            return 0xFF; //TODO
        else if (address == kNR52)
        {
            byte result = 0x70; // Unused bits are 1
            if (sound_enabled_)
                result |= 0x80; // Bit 7 - Sound on/off

            if (square_wave1_.GetEnabled())
                result |= 0x1;
            if (square_wave2_.GetEnabled())
                result |= 0x2;

            return result;
        }
        else if (address <= 0xFF2F)
            return 0xFF;
        else
            return wave_ram_[address - 0xFF30];
    }

    std::set<word> APU::GetAddresses() const
    {
        std::set<word> addresses;
        for (word i = 0xFF10; i <= 0xFF26; i++)
        {
            addresses.insert(i);
        }

        for (word i = 0xFF30; i <= 0xFF3F; i++)
        {
            addresses.insert(i);
        }

        return addresses;
    }

    void APU::Tick()
    {
        //samples_[0] = square_wave1_.Tick();
        samples_[1] = square_wave2_.Tick();

        // Panning
        word left = 0, right = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (channel_left_enabled_[i])
                left += samples_[i];
            if (channel_right_enabled_[i])
                right += samples_[i];
        }

        // Volume
        left *= (left_volume_ + 1);
        right *= (right_volume_ + 1);

        // Mixing
        left /= 4;
        right /= 4;

        if (output_handler_)
            output_handler_->Play((byte)left, (byte)right);
    }

    void APU::SetOutputHandler(std::shared_ptr<APU::OutputHandler> handler)
    {
        output_handler_ = handler;
    }
} // namespace gandalf