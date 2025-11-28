#pragma once

#include "../juce_header.h"
#include "../instrumental_track/instrumental_track.h"
#include "../audio_track/audio_track.h"
#include <vector>

class TrackHandler {
    public:
        TrackHandler() = default;
        ~TrackHandler() = default;
        void save(std::string);
        void render(std::string);
        void add_track(InstrumentalTrack&&);
        void add_track(AudioTrack&&);
        void remove_track(InstrumentalTrack);
        void remove_track(AudioTrack);
    private:
        std::vector<InstrumentalTrack> m_instrumental_tracks;
        std::vector<AudioTrack> m_audio_tracks;    
};
