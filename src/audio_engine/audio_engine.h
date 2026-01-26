#pragma once
#include <memory>
#include "../juce_header.h"
#include "../../external/JUCE/modules/juce_audio_devices/juce_audio_devices.h"
#include "track_handler/track_handler.h"
#include "../queue/queue.h"
#include "../queue/midi_command.h"

class AudioEngine : juce::AudioIODeviceCallback {
    public:
        explicit AudioEngine(std::shared_ptr<Queue<MidiCommand, 1024>>);
        ~AudioEngine();
        void audioDeviceIOCallback(const float** inputChannelData,
                               int numInputChannels,
                               float** outputChannelData,
                               int numOutputChannels,
                               int numSamples);
    
        void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
        void audioDeviceStopped() override;

        std::unique_ptr<TrackHandler> getTrackHandler();        
        void start();
        void stop();
    private:
        std::shared_ptr<Queue<MidiCommand, 1024>> m_queue;
        std::unique_ptr<TrackHandler> m_track_handler;
        std::unique_ptr<juce::AudioDeviceManager> m_audio_device_manager;
};
