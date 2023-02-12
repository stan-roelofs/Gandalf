#include "audio_handler.h"

#include <iostream>

#include <SDL.h>

#include <gandalf/constants.h>

namespace
{
    constexpr int kFrequency = 44100;
    constexpr int kFormat = AUDIO_F32;
    constexpr int kNumberOfChannels = 2;
    constexpr int kBufferSizeSamples = 1024;
    constexpr int kBufferSizeBytes = kBufferSizeSamples * kNumberOfChannels * SDL_AUDIO_BITSIZE(kFormat) / 8;
    constexpr int kDownSampling = gandalf::CPUFrequency / kFrequency; // TODO how to call this constant...
}

SDLAudioHandler::SDLAudioHandler(const bool& wait, const bool& gb_thread_running) :
    device_id_(0),
    index_(0),
    wait_(wait),
    gb_thread_running_(gb_thread_running)
{
    static_assert(sizeof(float) == 4);
    audio_buffer_.resize(kBufferSizeBytes);

    SDL_AudioSpec desired_spec;
    memset(&desired_spec, 0, sizeof(desired_spec));
    desired_spec.freq = kFrequency;
    desired_spec.format = kFormat;
    desired_spec.channels = kNumberOfChannels;
    desired_spec.samples = kBufferSizeSamples;

    SDL_AudioSpec obtained_spec;
    device_id_ = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, 0);


    if (device_id_ == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    // TODO compare desired to obtained
    SDL_PauseAudioDevice(device_id_, 0);
}

SDLAudioHandler::~SDLAudioHandler()
{
    SDL_CloseAudioDevice(device_id_);
}

uint32_t SDLAudioHandler::GetNextSampleTime()
{
    return kDownSampling;
}

void SDLAudioHandler::Play(float left, float right)
{
    audio_buffer_[index_++] = left;
    audio_buffer_[index_++] = right;

    if (index_ == kBufferSizeSamples * kNumberOfChannels)
    {
        if (wait_) {
            while (gb_thread_running_ && wait_ && SDL_GetQueuedAudioSize(device_id_) > kBufferSizeBytes) {}

            SDL_QueueAudio(device_id_, audio_buffer_.data(), kBufferSizeBytes);
        }

        index_ = 0;
    }
}