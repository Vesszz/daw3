#pragma once
#include "../track_handler/track_handler.h"
#include "../../external/JUCE/modules/juce_audio_devices/juce_audio_devices.h"
#include <memory>

// TODO: std::unique_ptr<AudioEngine> which contains track handler and adm

class DAW3 {
    public:
        DAW3();
        ~DAW3() = default;
        juce::AudioDeviceManager& getAudioDeviceManager();
        void add_track(InstrumentalTrack&&);
        void add_track(AudioTrack&&);
    private:        
        std::unique_ptr<TrackHandler> m_track_handler;
        std::unique_ptr<juce::AudioDeviceManager> m_audio_device_manager;
};
