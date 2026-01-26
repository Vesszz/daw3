#pragma once
#include <vector>
#include <cstdint>
#include "../../juce_header.h"

struct PianoRollNote {
    int pitch = 60;
    double startBeat = 0.0;
    double lengthBeats = 1.0;
    juce::uint8 velocity = 100;
    
    juce::Rectangle<int> getBounds(int pixelsPerBeat, int keyHeight, 
                                   int pianoRollWidth, int pianoRollHeight) const {
        int x = juce::roundToInt(startBeat * pixelsPerBeat);
        int y = (127 - pitch) * keyHeight;
        int width = juce::jmax(4, juce::roundToInt(lengthBeats * pixelsPerBeat));
        int height = keyHeight;
        return {x, y, width, height};
    }
};

class PianoRollData {
public:
    PianoRollData() = default;
    
    void addNote(int pitch, double startBeat, double lengthBeats, juce::uint8 velocity = 100) {
        PianoRollNote note;
        note.pitch = juce::jlimit(0, 127, pitch);
        note.startBeat = startBeat;
        note.lengthBeats = juce::jmax(0.1, lengthBeats);
        note.velocity = velocity;
        notes.push_back(note);
    }
    
    void clear() { notes.clear(); }
    
    std::vector<PianoRollNote> getNotes() const { return notes; }
    
    PianoRollNote* getNoteAtPosition(int x, int y, int pixelsPerBeat, int keyHeight) {
        for (auto& note : notes) {
            auto bounds = note.getBounds(pixelsPerBeat, keyHeight, 10000, 10000);
            if (bounds.contains(x, y)) return &note;
        }
        return nullptr;
    }
    
    void removeNote(PianoRollNote* noteToRemove) {
        notes.erase(std::remove_if(notes.begin(), notes.end(),
            [noteToRemove](const PianoRollNote& n) {
                return &n == noteToRemove;
            }), notes.end());
    }
    
    juce::MidiMessageSequence toMidiSequence(double tempoBpm = 120.0) const {
        juce::MidiMessageSequence seq;
        double secondsPerBeat = 60.0 / tempoBpm;
        
        for (const auto& note : notes) {
            double startTime = note.startBeat * secondsPerBeat;
            double endTime = (note.startBeat + note.lengthBeats) * secondsPerBeat;
            
            seq.addEvent(juce::MidiMessage::noteOn(1, note.pitch, note.velocity), startTime);
            seq.addEvent(juce::MidiMessage::noteOff(1, note.pitch), endTime);
        }
        
        seq.updateMatchedPairs();
        return seq;
    }
    
private:
    std::vector<PianoRollNote> notes;
};
