#include <gandalf/apu.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/exception.h>
#include <gandalf/util.h>

#include "sound/square_wave_channel.h"
#include "sound/noise_channel.h"
#include "sound/wave_channel.h"

namespace gandalf
{
    APU::APU(std::shared_ptr<APU::OutputHandler> audio_handler): Memory::AddressHandler("APU"),
        output_handler_(audio_handler),
        ticks_until_sample_(audio_handler ? audio_handler->GetNextSampleTime() : 0),
        vin_left_(false),
        vin_right_(false),
        left_volume_(0),
        right_volume_(0),
        sound_enabled_(false)
    {
        wave_ram_.fill((byte)std::rand());
        samples_.fill(0);

        channel_left_enabled_.fill(false);
        channel_right_enabled_.fill(false);

        mute_channel_.fill(false);


        //todo dac

        sound_channels_[0] = std::make_unique<SquareWaveChannel>(frame_sequencer_, true);
        sound_channels_[1] = std::make_unique<SquareWaveChannel>(frame_sequencer_, false);
        sound_channels_[2] = std::make_unique<WaveChannel>(frame_sequencer_, wave_ram_);
        sound_channels_[3] = std::make_unique<NoiseChannel>(frame_sequencer_);
    }

    APU::~APU() = default;

    void APU::Write(word address, byte value)
    {
        assert(BETWEEN(address, 0xFF10, 0xFF27) || BETWEEN(address, 0xFF30, 0xFF40));

        if (address <= kNR44)
        {
            const int channel = (address - kNR10) / 5;
            const int reg = (address - kNR10) % 5;
            sound_channels_[channel]->SetRegister(reg, value);
        }
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
            sound_enabled_ = (value & 0x80) != 0;
        else if (address >= 0xFF30)
            wave_ram_[address - 0xFF30] = value;
    }

    byte APU::Read(word address) const
    {
        assert(BETWEEN(address, 0xFF10, 0xFF27) || BETWEEN(address, 0xFF30, 0xFF40));
        if (address <= kNR44)
        {
            const int channel = (address - kNR10) / 5;
            const int reg = (address - kNR10) % 5;
            return sound_channels_[channel]->GetRegister(reg);
        }
        else if (address == kNR50)
        {
            byte result = 0;
            result |= vin_left_ ? 0x80 : 0;
            result |= vin_right_ ? 0x08 : 0;
            result |= left_volume_;
            result |= right_volume_ << 4;
            return result;
        }
        else if (address == kNR51)
        {
            byte result = 0;
            for (int i = 0; i < 4; ++i)
                result |= channel_right_enabled_[i] ? (1 << i) : 0;

            for (int i = 0; i < 4; ++i)
                result |= channel_left_enabled_[i] ? (1 << (i + 4)) : 0;
            return result;
        }
        else if (address == kNR52)
        {
            byte result = 0x70; // Unused bits are 1
            if (sound_enabled_)
                result |= 0x80; // Bit 7 - Sound on/off

            for (int i = 0; i < 4; ++i) {
                if (sound_channels_[i]->GetEnabled())
                    result |= (1 << i);
            }

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
        frame_sequencer_.Tick();

        for (int i = 0; i < 4; ++i) {
            samples_[i] = sound_channels_[i]->Tick();
            assert(samples_[i] <= 15);
        }

        if (!output_handler_)
            return;

        --ticks_until_sample_;
        if (ticks_until_sample_ > 0)
            return;

        ticks_until_sample_ = output_handler_->GetNextSampleTime();
        if (ticks_until_sample_ == 0)
            throw Exception("Next sample time must be greater than 0");

        for (int i = 0; i < 4; ++i)
        {
            if (mute_channel_[i])
                samples_[i] = 0;
        }

        // Panning
        float left = 0, right = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (channel_left_enabled_[i])
                left += (samples_[i] / 7.5f) - 1.0f; // Scale volume from 0..15 to -1..1
            if (channel_right_enabled_[i])
                right += (samples_[i] / 7.5f) - 1.0f; // Scale volume from 0..15 to -1..1
        }

        // Mixing and volume
        left = (left * (left_volume_ + 1) / 8.f) / 4.f;
        right = (right * (right_volume_ + 1) / 8.f) / 4.f;

        output_handler_->Play(left, right);
    }

    void APU::MuteChannel(int channel, bool mute)
    {
        if (channel >= 4)
            return;

        mute_channel_[channel] = mute;
    }
} // namespace gandalf