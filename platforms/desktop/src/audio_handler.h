#ifndef __GANDALF_AUDIO_HANDLER_H
#define __GANDALF_AUDIO_HANDLER_H

#include <SDL.h>

#include <gandalf/apu.h>

class SDLAudioHandler : public gandalf::APU::OutputHandler
{
public:
    SDLAudioHandler();
    virtual ~SDLAudioHandler();

    void Play(gandalf::byte left, gandalf::byte right) override;

    bool limit_frames_;

private:
    SDL_AudioDeviceID device_id_;
    std::vector<std::int8_t> audio_buffer_;
    int index_;
    int divider_;
};

#endif