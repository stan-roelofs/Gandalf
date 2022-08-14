#include "audio_handler.h"

#include <iostream>

#include <SDL.h>

#include <gandalf/constants.h>

namespace
{
    constexpr int kFrequency = 48000;
    constexpr int kFormat = AUDIO_U8;
    constexpr int kNumberOfChannels = 2;
    constexpr int kBufferSizeSamples = 1024;
    constexpr int kBufferSizeBytes = kBufferSizeSamples * kNumberOfChannels;
    constexpr int kDownSampling = gandalf::kCPUFrequency / kFrequency; // TODO how to call this constant...
}

SDLAudioHandler::SDLAudioHandler() : device_id_(0), index_(0), divider_(0), limit_frames_(true)
{
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


void SDLAudioHandler::Play(gandalf::byte left, gandalf::byte right)
{
    if (divider_++ < kDownSampling)
        return;

    divider_ = 0;

    audio_buffer_[index_++] = left;
    audio_buffer_[index_++] = right;

    if (index_ == kBufferSizeBytes / 2)
    {
        if (limit_frames_) {
            while (limit_frames_ && SDL_GetQueuedAudioSize(device_id_) > kBufferSizeBytes / 2) {}

            SDL_QueueAudio(device_id_, audio_buffer_.data(), kBufferSizeBytes / 2);
        }

        index_ = 0;
    }
}