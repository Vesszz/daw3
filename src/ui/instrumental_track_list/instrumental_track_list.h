#pragma once

#include "../../juce_header.h"
#include "../../audio_engine/instrumental_track/instrumental_track.h"
#include <vector>
#include <memory>

class InstrumentalTrackList : public juce::Component,
                              public juce::ListBoxModel {
public:
    InstrumentalTrackList();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    
    void addTrack(std::shared_ptr<InstrumentalTrack> track);
    InstrumentalTrack* getTrack(int index);
    int getNumTracks() const;
    
    void clearSelection();

    std::function<void(int trackIndex)> onTrackSelected;
    
private:
    std::vector<std::shared_ptr<InstrumentalTrack>> m_tracks;
    juce::ListBox m_listBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentalTrackList)
};
