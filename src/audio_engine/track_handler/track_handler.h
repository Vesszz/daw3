#pragma once

#include "../../juce_header.h"
#include "../instrumental_track/instrumental_track.h"
#include "../../queue/midi_command.h"
#include <vector>
#include <memory>

/**
* @brief Хранит внутри себя дорожки и перенаправляет запросы с AudioEnginе'а в конкретную дорожку 
*/
class TrackHandler {
    public:
        TrackHandler();
        ~TrackHandler();
        
        void prepareToPlay(double sampleRate, int blockSize);
        void releaseResources();
        
        void renderAudio(juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiBuffer);
        
        void addTrack(std::shared_ptr<InstrumentalTrack> track);
        InstrumentalTrack* addTrack(std::string name, std::unique_ptr<juce::AudioPluginInstance> plugin);
        void removeTrack(size_t index);
        size_t getNumTracks() const;
        InstrumentalTrack* getTrack(size_t index);
        
    private:
        std::vector<std::shared_ptr<InstrumentalTrack>> m_instrumental_tracks;
        juce::AudioBuffer<float> m_mixBuffer;
        double m_sampleRate = 44100.0;
        int m_blockSize = 512;
};
