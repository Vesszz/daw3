#include "track_handler.h"

TrackHandler::TrackHandler() {
    m_mixBuffer.setSize(2, 512);
}

TrackHandler::~TrackHandler() {
    releaseResources();
}

void TrackHandler::prepareToPlay(double sampleRate, int blockSize) {
    m_sampleRate = sampleRate;
    m_blockSize = blockSize;
    m_mixBuffer.setSize(2, blockSize);
    
    for (auto& track : m_instrumental_tracks) {
        if (track) {
            track->prepareToPlay(sampleRate, blockSize);
        }
    }
}

void TrackHandler::releaseResources() {
    for (auto& track : m_instrumental_tracks) {
        if (track) {
            track->releaseResources();
        }
    }
}

void TrackHandler::renderAudio(juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiBuffer) {
    m_mixBuffer.clear();
    
    if (m_instrumental_tracks.empty()) {
        audioBuffer.clear();
        return;
    }
    
    std::vector<juce::MidiBuffer> trackMidiBuffers(m_instrumental_tracks.size());
    
    for (const auto metadata : midiBuffer) {
        auto msg = metadata.getMessage();
        int samplePos = metadata.samplePosition;
        
        // FIX 
        int targetTrack = 0;
        
        if (targetTrack >= 0 && targetTrack < m_instrumental_tracks.size()) {
            trackMidiBuffers[targetTrack].addEvent(msg, samplePos);
        }
    }
    
    for (size_t i = 0; i < m_instrumental_tracks.size(); ++i) {
        if (!m_instrumental_tracks[i]) continue;
        
        juce::AudioBuffer<float> trackBuffer(m_mixBuffer.getNumChannels(), 
                                             m_mixBuffer.getNumSamples());
        trackBuffer.clear();
        
    
        m_instrumental_tracks[i]->renderAudio(trackBuffer, trackMidiBuffers[i]);
        
        for (int ch = 0; ch < m_mixBuffer.getNumChannels(); ++ch) {
            m_mixBuffer.addFrom(ch, 0, trackBuffer, ch, 0, trackBuffer.getNumSamples());
        }
    }
    
    for (int ch = 0; ch < std::min(m_mixBuffer.getNumChannels(), audioBuffer.getNumChannels()); ++ch) {
        audioBuffer.copyFrom(ch, 0, m_mixBuffer, ch, 0, audioBuffer.getNumSamples());
    }
}

void TrackHandler::addTrack(std::shared_ptr<InstrumentalTrack> track) {
    if (!track) return;
    
    if (m_sampleRate > 0 && m_blockSize > 0) {
        track->prepareToPlay(m_sampleRate, m_blockSize);
    }
    
    m_instrumental_tracks.push_back(track);
}

InstrumentalTrack* TrackHandler::addTrack(std::string name, std::unique_ptr<juce::AudioPluginInstance> plugin) {
    if (!plugin) return nullptr;
    
    auto track = std::make_shared<InstrumentalTrack>(std::move(name), std::move(plugin));
    InstrumentalTrack* trackPtr = track.get();
    
    addTrack(track);
    return trackPtr;
}

void TrackHandler::removeTrack(size_t index) {
    if (index < m_instrumental_tracks.size()) {
        m_instrumental_tracks.erase(m_instrumental_tracks.begin() + index);
    }
}

size_t TrackHandler::getNumTracks() const {
    return m_instrumental_tracks.size();
}

InstrumentalTrack* TrackHandler::getTrack(size_t index) {
    if (index < m_instrumental_tracks.size()) {
        return m_instrumental_tracks[index].get();
    }
    return nullptr;
}
