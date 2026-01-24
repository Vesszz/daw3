#pragma once
#include "../../juce_header.h"

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
        PianoRoll();
        ~PianoRoll() override;

        void paint(juce::Graphics&) override;
        void resized() override;
        
        void mouseDown(const juce::MouseEvent&) override;
        void mouseDrag(const juce::MouseEvent&) override;
        void mouseUp(const juce::MouseEvent&) override;
        
        juce::MidiMessageSequence export_to_midi_sequence() const;
        void import_from_midi_sequence(const juce::MidiMessageSequence& sequence);
        
        void addNote(int pitch, double startBeat, double lengthBeats, juce::uint8 velocity = 100);
        void clearAllNotes();
        
        void setTempo(double bpm);
        void setPixelsPerBeat(int ppb); 
    private:
        struct Note {
            int pitch = 60;
            double startBeat = 0.0;
            double lengthBeats = 1.0;
            juce::uint8 velocity = 100;
            
            juce::Rectangle<int> getBounds(int pixelsPerBeat, int keyHeight, int pianoRollWidth, int pianoRollHeight) const;
        };
        
        std::vector<Note> notes;
        
        Note* resizingNote = nullptr;
        int resizeStartX = 0;
        double resizeStartLength = 0.0;
        
        int pixelsPerBeat = 80;
        int keyHeight = 16;
        int numOctaves = 8;
        int lowestNote = 12; // C1
        
        double tempoBpm = 120.0;
        int timeSigNumerator = 4;
        int timeSigDenominator = 4;
        
        juce::Viewport viewport;
        ContentComponent content;
        juce::ScrollBar horizontalScrollBar;
        juce::ScrollBar verticalScrollBar;
        
        void drawGrid(juce::Graphics& g);
        void drawNotes(juce::Graphics& g);
        void drawPlayhead(juce::Graphics& g);
        void drawKeyboard(juce::Graphics& g);
        
        Note* getNoteAtPosition(int x, int y);
        int getPitchFromY(int y) const;
        double getBeatFromX(int x) const;
        int getXFromBeat(double beat) const;
        int getYFromPitch(int pitch) const;
        
        void updateContentSize();
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRoll)
};
