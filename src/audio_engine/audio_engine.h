#pragma once
#include <memory>
#include "../juce_header.h"
#include "../../external/JUCE/modules/juce_audio_devices/juce_audio_devices.h"
#include "track_handler/track_handler.h"

class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine() = default;
    private:
        std::unique_ptr<TrackHandler> m_track_handler;
        std::unique_ptr<juce::AudioDeviceManager> m_audio_device_manager;

};
