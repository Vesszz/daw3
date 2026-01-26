#pragma once
#include "../../juce_header.h"
#include "../../queue/queue.h"
#include "../../queue/midi_command.h"
#include "pianoroll_data.h"
#include <memory>
#include <unordered_map>

class ContentComponent : public juce::Component {
public:
    std::function<void(juce::Graphics&)> onCustomPaint;
    std::function<void(const juce::MouseEvent&)> onMouseDown;
    std::function<void(const juce::MouseEvent&)> onMouseDrag;
    std::function<void(const juce::MouseEvent&)> onMouseUp;
    
    void paint(juce::Graphics& g) override {
        if (onCustomPaint) onCustomPaint(g);
    }
    
    void mouseDown(const juce::MouseEvent& e) override {
        if (onMouseDown) onMouseDown(e);
    }
    
    void mouseDrag(const juce::MouseEvent& e) override {
        if (onMouseDrag) onMouseDrag(e);
    }
    
    void mouseUp(const juce::MouseEvent& e) override {
        if (onMouseUp) onMouseUp(e);
    }
};

class PianoRoll : public juce::Component {
public:
    explicit PianoRoll(std::shared_ptr<Queue<MidiCommand, 1024>>, uint16_t initialTrackID = 0);
    ~PianoRoll() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void setCurrentTrackID(uint16_t trackID);
    uint16_t getCurrentTrackID() const { return m_currentTrackID; }
    
    void addNote(int pitch, double startBeat, double lengthBeats, juce::uint8 velocity = 100);
    void clearCurrentTrackNotes();
    
    juce::MidiMessageSequence exportCurrentTrackToMidi(double tempoBpm = 120.0) const;
    
    PianoRollData* getTrackData(uint16_t trackID);
    const PianoRollData* getTrackData(uint16_t trackID) const;
    
    void setTempo(double bpm) { tempoBpm = bpm; }
    void setPixelsPerBeat(int ppb) { 
        pixelsPerBeat = ppb; 
        updateContentSize(); 
        repaint();
    }
    
private:
    std::unordered_map<uint16_t, PianoRollData> m_trackData;
    uint16_t m_currentTrackID = 0;
    
    int pixelsPerBeat = 80;
    int keyHeight = 16;
    int numOctaves = 8;
    int lowestNote = 12;
    
    double tempoBpm = 120.0;
    int timeSigNumerator = 4;
    int timeSigDenominator = 4;
    
    juce::Viewport viewport;
    ContentComponent content;
    juce::ScrollBar horizontalScrollBar;
    juce::ScrollBar verticalScrollBar;
    
    std::shared_ptr<Queue<MidiCommand, 1024>> m_midi_queue;
    
    PianoRollNote* m_resizingNote = nullptr;
    int m_resizeStartX = 0;
    double m_resizeStartLength = 0.0;
    int m_currentPlayingNote = -1;
    
    void drawGrid(juce::Graphics& g);
    void drawNotes(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g);
    void drawKeyboard(juce::Graphics& g);
    
    PianoRollNote* getNoteAtPosition(int x, int y);
    int getPitchFromY(int y) const;
    double getBeatFromX(int x) const;
    int getXFromBeat(double beat) const;
    int getYFromPitch(int pitch) const;
    
    void updateContentSize();
    void ensureTrackExists(uint16_t trackID);
    void deleteNoteAtPosition(int x, int y);
    
    void sendNoteOn(int pitch, int velocity = 100);
    void sendNoteOff(int pitch);
    void stopCurrentNote();
    uint64_t getCurrentTimestamp() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRoll)
};
