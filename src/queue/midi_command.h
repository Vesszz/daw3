#pragma once

#include "../juce_header.h"
#include <cstdint>

struct MidiCommand {
    enum Type : uint8_t {
        NoteOn = 0,
        NoteOff = 1,
        ControlChange = 2,
        ProgramChange = 3,
        PitchBend = 4,
        Aftertouch = 5,
        AllNotesOff = 6
    };
    
    Type type;
    uint8_t channel;    // MIDI channel 0-15
    uint8_t note;       // MIDI note number 0-127
    uint8_t velocity;   // 0-127 (для NoteOn), 0 у NoteOff
    uint8_t controller; // для ControlChange
    uint16_t value;     // для ControlChange/PitchBend (0-16383)
    uint16_t trackID;   // ID дорожки
    uint64_t timestamp; // в микросекундах
    
    MidiCommand() = default;
    
    MidiCommand(Type t, uint8_t ch, uint8_t n, uint8_t vel, uint64_t ts = 0)
        : type(t), channel(ch), note(n), velocity(vel), controller(0), value(0), timestamp(ts) {}
    
    MidiCommand(Type t, uint8_t ch, uint8_t cc, uint16_t val, uint64_t ts = 0)
        : type(t), channel(ch), note(0), velocity(0), controller(cc), value(val), timestamp(ts) {}
    
    static MidiCommand createNoteOn(uint8_t ch, uint8_t note, uint8_t velocity, uint64_t ts = 0) {
        return {NoteOn, ch, note, velocity, ts};
    }
    
    static MidiCommand createNoteOff(uint8_t ch, uint8_t note, uint8_t velocity = 0, uint64_t ts = 0) {
        return {NoteOff, ch, note, velocity, ts};
    }
    
    static MidiCommand createControlChange(uint8_t ch, uint8_t controller, uint8_t value, uint64_t ts = 0) {
        return {ControlChange, ch, controller, static_cast<uint16_t>(value), ts};
    }
    
    static MidiCommand createAllNotesOff(uint8_t ch, uint64_t ts = 0) {
        MidiCommand cmd;
        cmd.type = AllNotesOff;
        cmd.channel = ch;
        cmd.timestamp = ts;
        return cmd;
    }
    
    bool isNoteOn() const { return type == NoteOn; }
    bool isNoteOff() const { return type == NoteOff; }
    bool isControlChange() const { return type == ControlChange; }
    bool isAllNotesOff() const { return type == AllNotesOff; }
    
    juce::MidiMessage toJuceMidiMessage() const {
        switch (type) {
            case NoteOn:
                return juce::MidiMessage::noteOn(channel + 1, note, juce::uint8(velocity));
            case NoteOff:
                return juce::MidiMessage::noteOff(channel + 1, note, juce::uint8(velocity));
            case ControlChange:
                return juce::MidiMessage::controllerEvent(channel + 1, controller, juce::uint8(value));
            case ProgramChange:
                return juce::MidiMessage::programChange(channel + 1, juce::uint8(value));
            case PitchBend:
                return juce::MidiMessage::pitchWheel(channel + 1, value);
            case Aftertouch:
                return juce::MidiMessage::aftertouchChange(channel + 1, note, juce::uint8(value));
            case AllNotesOff:
                return juce::MidiMessage::allNotesOff(channel + 1);
            default:
                return juce::MidiMessage();
        }
    }
    
    static constexpr size_t getSizeForType(Type t) {
        switch (t) {
            case NoteOn:
            case NoteOff:
            case Aftertouch:
                return 3;  // channel + note + velocity/value
            case ControlChange:
            case ProgramChange:
                return 2;  // channel + controller/value
            case PitchBend:
                return 3;  // channel + value (14-bit)
            case AllNotesOff:
                return 1;  // только channel
            default:
                return 0;
        }
    }
};
