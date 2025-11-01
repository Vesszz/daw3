#pragma once

#include "juce_header.h"
#include "instrumental_track.h"
#include "audio_track.h"
#include <vector>

class TrackHandler {
    public:
        TrackHandler();
        ~TrackHandler();
        void save(std::string);
        void render(std::string);
        void add_track(InstrumentalTrack);
        void add_track(AudioTrack);
        void remove_track(InstrumentalTrack);
        void remove_track(AudioTrack);
    private:
        std::vector<InstrumentalTrack> instrumental_tracks;
        std::vector<AudioTrack> audio_tracks;    
};
