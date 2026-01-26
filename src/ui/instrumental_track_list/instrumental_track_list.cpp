#include "instrumental_track_list.h"

InstrumentalTrackList::InstrumentalTrackList() {
    m_listBox.setModel(this);
    m_listBox.setMultipleSelectionEnabled(false);
    addAndMakeVisible(m_listBox);
}

void InstrumentalTrackList::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void InstrumentalTrackList::resized() {
    m_listBox.setBounds(getLocalBounds());
}

int InstrumentalTrackList::getNumRows() {
    return static_cast<int>(m_tracks.size());
}

void InstrumentalTrackList::clearSelection() {
    m_listBox.deselectAllRows();
    m_listBox.repaint();
}

void InstrumentalTrackList::paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected) {
    if (row < 0 || row >= m_tracks.size()) return;
    
    if (selected) {
        g.fillAll(juce::Colours::lightblue);
    }
    
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    
    if (m_tracks[row]) {
        g.drawText(juce::String(row + 1) + ". " + m_tracks[row]->info(),
                  10, 0, w - 20, h, juce::Justification::centredLeft);
    }
}

void InstrumentalTrackList::selectedRowsChanged(int lastRowSelected) {
    if (onTrackSelected && lastRowSelected >= 0) {
        onTrackSelected(lastRowSelected);
    }
}

void InstrumentalTrackList::addTrack(std::shared_ptr<InstrumentalTrack> track) {
    m_tracks.push_back(track);
    m_listBox.updateContent();
}

InstrumentalTrack* InstrumentalTrackList::getTrack(int index) {
    if (index >= 0 && index < m_tracks.size()) {
        return m_tracks[index].get();
    }
    return nullptr;
}

int InstrumentalTrackList::getNumTracks() const {
    return static_cast<int>(m_tracks.size());
}
