#include "track_handler.h"

#include "../instrumental_track/instrumental_track.h"

void TrackHandler::add_track(InstrumentalTrack&& it) {
    m_instrumental_tracks.emplace_back(std::move(it));
}
