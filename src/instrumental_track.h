#pragma once

#include "juce_header.h"
#include "../external/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
#include "../external/JUCE/modules/juce_audio_processors/juce_audio_processors.h"

class InstrumentalTrack {
    public:
        InstrumentalTrack();
        ~InstrumentalTrack();
        std::string info();
        void add_midi_message(const juce::MidiMessage&, double);
    private:
        std::string m_name;
        juce::AudioPluginInstance m_audio_plugin_instance;
        juce::MidiMessageSequence m_midi_message_sequence;
};
