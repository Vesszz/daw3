#include "audio_engine.h"
#include "track_handler/track_handler.h"
#include <memory>
#include <stdexcept>

AudioEngine::AudioEngine(std::shared_ptr<Queue<MidiCommand, 1024>> q)
    : m_queue(std::move(q)) {
    m_audio_device_manager = std::make_unique<juce::AudioDeviceManager>();
    m_track_handler = std::make_unique<TrackHandler>();
}

AudioEngine::~AudioEngine() {
    stop();
}

std::unique_ptr<TrackHandler> AudioEngine::getTrackHandler() {
    return std::move(m_track_handler);
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData,
                                        int numInputChannels,
                                        float** outputChannelData,
                                        int numOutputChannels,
                                        int numSamples) {

    for (int ch = 0; ch < numOutputChannels; ++ch) {
        if (outputChannelData[ch] != nullptr) {
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);
        }
    }
    
    MidiCommand cmd;
    juce::MidiBuffer midiBuffer;
    
    while (m_queue->try_pop(cmd)) {
        juce::MidiMessage juceMsg = cmd.toJuceMidiMessage();
        midiBuffer.addEvent(juceMsg, 0);
    }
    if (!midiBuffer.isEmpty() && m_track_handler) {
        juce::AudioBuffer<float> audioBuffer(outputChannelData, numOutputChannels, numSamples);
        m_track_handler->renderAudio(audioBuffer, midiBuffer);
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device) {

}

void AudioEngine::audioDeviceStopped() {
    // m_track_handler->releaseResources();
}

void AudioEngine::start() {
    juce::String error = m_audio_device_manager->initialise(0, 2, nullptr, true);
    if (error.isNotEmpty()) {
        throw std::runtime_error("Failed to initialise audio device: " + error.toStdString());
    }

    auto* device = m_audio_device_manager.get()->getCurrentAudioDevice();
    device->start(nullptr);
    m_audio_device_manager->addAudioCallback(this);
}


void AudioEngine::stop() {
    m_audio_device_manager->removeAudioCallback(this);
    m_audio_device_manager->closeAudioDevice();
}
