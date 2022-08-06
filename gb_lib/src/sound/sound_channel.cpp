#include <gandalf/sound/sound_channel.h>

namespace gandalf
{
    SoundChannel::SoundChannel() : channel_enabled_(false)
    {
    }

    SoundChannel::~SoundChannel() = default;

    bool SoundChannel::GetEnabled() const
    {
        return channel_enabled_;
    }
} // namespace gandalf