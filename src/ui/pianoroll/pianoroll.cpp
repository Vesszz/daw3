#include "pianoroll.h"

PianoRoll::PianoRoll() : horizontalScrollBar(false), verticalScrollBar(true) {
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
        mouseDown(e);
    };

    content.onMouseDrag = [this](const juce::MouseEvent& e) {
        mouseDrag(e);
    };

    content.onMouseUp = [this](const juce::MouseEvent& e) {
        mouseUp(e);
    };
    
    addNote(60, 0.0, 2.0);
    addNote(64, 2.0, 1.0);
    addNote(67, 3.0, 1.5);
    
    updateContentSize();
}

PianoRoll::~PianoRoll() {}

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

void PianoRoll::mouseDown(const juce::MouseEvent& event) {
    if (event.mods.isRightButtonDown()) {
        Note* clickedNote = getNoteAtPosition(event.x, event.y);
        if (clickedNote) {
            notes.erase(std::remove_if(notes.begin(), notes.end(),
                [clickedNote](const Note& n) {
                    return &n == clickedNote;
                }), notes.end());
            content.repaint();
        }
    }
    else if (event.mods.isLeftButtonDown()) {
        Note* clickedNote = getNoteAtPosition(event.x, event.y);
        if (clickedNote) {
            auto bounds = clickedNote->getBounds(pixelsPerBeat, keyHeight,content.getWidth(), content.getHeight());
            int resizeHandleWidth = 8;
            
            if (event.x > bounds.getRight() - resizeHandleWidth) {
                // Ресайз ноты
                resizingNote = clickedNote;
                resizeStartX = event.x;
                resizeStartLength = clickedNote->lengthBeats;
            }
        } else {
            int pitch = getPitchFromY(event.y);
            double beat = getBeatFromX(event.x);
            addNote(pitch, beat, 1.0);
        }
        content.repaint();
    }
}

void PianoRoll::mouseDrag(const juce::MouseEvent& event) {
    if (resizingNote && event.mods.isLeftButtonDown()) {
        double deltaPixels = event.x - resizeStartX;
        double deltaBeats = deltaPixels / pixelsPerBeat;
        resizingNote->lengthBeats = juce::jmax(0.1, resizeStartLength + deltaBeats);
        content.repaint();
    }
}

void PianoRoll::mouseUp(const juce::MouseEvent&) {
    resizingNote = nullptr;
}

void PianoRoll::addNote(int pitch, double startBeat, double lengthBeats, juce::uint8 velocity) {
    Note newNote;
    newNote.pitch = juce::jlimit(0, 127, pitch);
    newNote.startBeat = startBeat;
    newNote.lengthBeats = juce::jmax(0.1, lengthBeats);
    newNote.velocity = velocity;
    notes.push_back(newNote);
    content.repaint();
}

void PianoRoll::clearAllNotes() {
    notes.clear();
    content.repaint();
}

void PianoRoll::setTempo(double bpm) {
    tempoBpm = bpm;
}

void PianoRoll::setPixelsPerBeat(int ppb) {
    pixelsPerBeat = ppb;
    updateContentSize();
    content.repaint();
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
    for (const auto& note : notes) {
        auto noteBounds = note.getBounds(pixelsPerBeat, keyHeight, content.getWidth(), content.getHeight());
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
    // TODO: implement playhead drawing
}

void PianoRoll::drawKeyboard(juce::Graphics& g) {
    // TODO: implement keyboard drawing
}

PianoRoll::Note* PianoRoll::getNoteAtPosition(int x, int y) {
    for (auto& note : notes) {
        auto bounds = note.getBounds(pixelsPerBeat, keyHeight, content.getWidth(), content.getHeight());
        if (bounds.contains(x, y)) return &note;
    }
    return nullptr;
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

juce::Rectangle<int> PianoRoll::Note::getBounds(int pixelsPerBeat, int keyHeight, int pianoRollWidth, int pianoRollHeight) const {
   int x = juce::roundToInt(startBeat * pixelsPerBeat);
    int y = (127 - pitch) * keyHeight;
    int width = juce::jmax(4, juce::roundToInt(lengthBeats * pixelsPerBeat));
    int height = keyHeight;
    return {x, y, width, height};
}
