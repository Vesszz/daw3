#include "pianoroll.h"
#include <cstdint>
#include <chrono>

PianoRoll::PianoRoll(std::shared_ptr<Queue<MidiCommand, 1024>> queue, uint16_t initialTrackID)
    : m_currentTrackID(initialTrackID), 
      m_midi_queue(std::move(queue)),
      horizontalScrollBar(false), 
      verticalScrollBar(true) {
    
    ensureTrackExists(m_currentTrackID);
    
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&content, false);
    viewport.setScrollBarsShown(true, true);
    
    addAndMakeVisible(horizontalScrollBar);
    addAndMakeVisible(verticalScrollBar);
    
    content.onCustomPaint = [this](juce::Graphics& g) {
        drawGrid(g);
        drawNotes(g);
    };
    
    content.onMouseDown = [this](const juce::MouseEvent& e) {
        if (e.mods.isRightButtonDown()) {
            deleteNoteAtPosition(e.x, e.y);
        }
        else if (e.mods.isLeftButtonDown()) {
            auto* clickedNote = getNoteAtPosition(e.x, e.y);
            if (clickedNote) {
                auto bounds = clickedNote->getBounds(pixelsPerBeat, keyHeight, 
                                                    content.getWidth(), content.getHeight());
                int resizeHandleWidth = 8;
                if (e.x > bounds.getRight() - resizeHandleWidth) {
                    m_resizingNote = clickedNote;
                    m_resizeStartX = e.x;
                    m_resizeStartLength = clickedNote->lengthBeats;
                }
            } else {
                int pitch = getPitchFromY(e.y);
                double beat = getBeatFromX(e.x);
                addNote(pitch, beat, 1.0);
            }
            content.repaint();
        }
    };

    content.onMouseDrag = [this](const juce::MouseEvent& e) {
        if (m_resizingNote && e.mods.isLeftButtonDown()) {
            double deltaPixels = e.x - m_resizeStartX;
            double deltaBeats = deltaPixels / pixelsPerBeat;
            m_resizingNote->lengthBeats = juce::jmax(0.1, m_resizeStartLength + deltaBeats);
            content.repaint();
        }
    };

    content.onMouseUp = [this](const juce::MouseEvent&) {
        m_resizingNote = nullptr;
        m_currentPlayingNote = -1;
    };
    
    auto& trackData = m_trackData[m_currentTrackID];
    trackData.addNote(60, 0.0, 2.0);
    trackData.addNote(64, 2.0, 1.0);
    trackData.addNote(67, 3.0, 1.5);
    
    updateContentSize();
}

PianoRoll::~PianoRoll() {
}

void PianoRoll::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xFF, 0x22, 0x22));
}

void PianoRoll::resized() {
    auto bounds = getLocalBounds();
    viewport.setBounds(bounds);
    horizontalScrollBar.setBounds(0, getHeight() - 16, getWidth() - 16, 16);
    verticalScrollBar.setBounds(getWidth() - 16, 0, 16, getHeight() - 16);
    updateContentSize();
}

void PianoRoll::setCurrentTrackID(uint16_t trackID) {
    if (trackID != m_currentTrackID) {
        m_currentTrackID = trackID;
        ensureTrackExists(trackID);
        content.repaint();
    }
}

void PianoRoll::ensureTrackExists(uint16_t trackID) {
    if (m_trackData.find(trackID) == m_trackData.end()) {
        m_trackData[trackID] = PianoRollData();
    }
}

void PianoRoll::addNote(int pitch, double startBeat, double lengthBeats, juce::uint8 velocity) {
    ensureTrackExists(m_currentTrackID);
    m_trackData[m_currentTrackID].addNote(pitch, startBeat, lengthBeats, velocity);
    content.repaint();
}

void PianoRoll::clearCurrentTrackNotes() {
    if (m_trackData.find(m_currentTrackID) != m_trackData.end()) {
        m_trackData[m_currentTrackID].clear();
        content.repaint();
    }
}

void PianoRoll::deleteNoteAtPosition(int x, int y) {
    auto* trackData = getTrackData(m_currentTrackID);
    if (!trackData) return;
    
    auto* note = trackData->getNoteAtPosition(x, y, pixelsPerBeat, keyHeight);
    if (note) {
        trackData->removeNote(note);
        content.repaint();
    }
}

PianoRollNote* PianoRoll::getNoteAtPosition(int x, int y) {
    auto* trackData = getTrackData(m_currentTrackID);
    if (!trackData) return nullptr;
    
    return trackData->getNoteAtPosition(x, y, pixelsPerBeat, keyHeight);
}

PianoRollData* PianoRoll::getTrackData(uint16_t trackID) {
    auto it = m_trackData.find(trackID);
    return (it != m_trackData.end()) ? &it->second : nullptr;
}

const PianoRollData* PianoRoll::getTrackData(uint16_t trackID) const {
    auto it = m_trackData.find(trackID);
    return (it != m_trackData.end()) ? &it->second : nullptr;
}

juce::MidiMessageSequence PianoRoll::exportCurrentTrackToMidi(double tempoBpm) const {
    auto* trackData = getTrackData(m_currentTrackID);
    if (!trackData) return juce::MidiMessageSequence();
    
    return trackData->toMidiSequence(tempoBpm);
}

void PianoRoll::drawGrid(juce::Graphics& g) {
    auto bounds = content.getLocalBounds();
    int totalBeats = 32;
    int totalKeys = numOctaves * 12;
    
    g.fillAll(juce::Colours::darkgrey);
    
    for (int beat = 0; beat <= totalBeats; ++beat) {
        int x = beat * pixelsPerBeat;
        if (beat % timeSigNumerator == 0) {
            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.drawVerticalLine(x, 0, bounds.getHeight());
        } else {
            g.setColour(juce::Colours::grey.withAlpha(0.5f));
            g.drawVerticalLine(x, 0, bounds.getHeight());
        }
    }
    
    for (int key = 0; key <= totalKeys; ++key) {
        int y = key * keyHeight;
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawHorizontalLine(y, 0, bounds.getWidth());
        
        int pitch = lowestNote + key;
        int noteInOctave = pitch % 12;
        if (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 ||
            noteInOctave == 8 || noteInOctave == 10) {
            g.setColour(juce::Colour(0x40, 0x40, 0x40));
            g.fillRect(0, y, bounds.getWidth(), keyHeight);
        }
    }
}

void PianoRoll::drawNotes(juce::Graphics& g) {
    auto* trackData = getTrackData(m_currentTrackID);
    if (!trackData) return;
    
    for (const auto& note : trackData->getNotes()) {
        auto noteBounds = note.getBounds(pixelsPerBeat, keyHeight, 
                                        content.getWidth(), content.getHeight());
        float velocityFactor = note.velocity / 127.0f;
        juce::Colour noteColour = juce::Colour::fromHSV(0.6f, 0.8f, velocityFactor * 0.7f + 0.3f, 1.0f);
        
        g.setColour(noteColour);
        g.fillRect(noteBounds);
        g.setColour(juce::Colours::black);
        g.drawRect(noteBounds, 1);
        
        juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        int octave = note.pitch / 12 - 1;
        int noteNameIndex = note.pitch % 12;
        
        g.setColour(juce::Colours::white);
        g.setFont(10.0f);
        g.drawText(noteNames[noteNameIndex] + juce::String(octave),
                  noteBounds.reduced(2),
                  juce::Justification::centred, true);
    }
}

void PianoRoll::drawPlayhead(juce::Graphics& g) {
}

void PianoRoll::drawKeyboard(juce::Graphics& g) {
}

int PianoRoll::getPitchFromY(int y) const {
    int key = y / keyHeight;
    int pitch = 127 - key;
    return juce::jlimit(0, 127, pitch);
}

double PianoRoll::getBeatFromX(int x) const {
    return x / (double)pixelsPerBeat;
}

int PianoRoll::getXFromBeat(double beat) const {
    return juce::roundToInt(beat * pixelsPerBeat);
}

int PianoRoll::getYFromPitch(int pitch) const {
    int key = pitch - lowestNote;
    return key * keyHeight;
}

void PianoRoll::updateContentSize() {
    int totalBeats = 32;
    int totalKeys = numOctaves * 12;
    content.setSize(totalBeats * pixelsPerBeat, totalKeys * keyHeight);
}

void PianoRoll::sendNoteOn(int pitch, int velocity) {
    if (!m_midi_queue) {
        return;
    }
    
    MidiCommand cmd;
    cmd.type = MidiCommand::NoteOn;
    cmd.channel = 0;
    cmd.note = static_cast<uint8_t>(pitch);
    cmd.velocity = static_cast<uint8_t>(velocity);
    cmd.timestamp = getCurrentTimestamp();
    cmd.trackID = m_currentTrackID;
    
    if (!m_midi_queue->try_push(std::move(cmd))) {
        m_midi_queue->push(std::move(cmd));
    }
}

void PianoRoll::sendNoteOff(int pitch) {
    if (!m_midi_queue) return;
    
    MidiCommand cmd;
    cmd.type = MidiCommand::NoteOff;
    cmd.channel = 0;
    cmd.note = static_cast<uint8_t>(pitch);
    cmd.velocity = 0;
    cmd.timestamp = getCurrentTimestamp();
    cmd.trackID = m_currentTrackID;
    
    m_midi_queue->push(std::move(cmd));
}

void PianoRoll::stopCurrentNote() {
    if (m_currentPlayingNote != -1) {
        sendNoteOff(m_currentPlayingNote);
        m_currentPlayingNote = -1;
    }
}

uint64_t PianoRoll::getCurrentTimestamp() const {
    using namespace std::chrono;
    static auto start_time = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    return duration_cast<microseconds>(now - start_time).count();
}
