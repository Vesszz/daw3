#pragma once

#include "../../juce_header.h"
#include "../../../external/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
#include "../../../external/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
#include <memory.h>
#include <string.h>

class InstrumentalTrack {
    public:
        static std::unique_ptr<juce::AudioPluginInstance> load_plugin(juce::File, float = 44100.0, int = 512); 
        InstrumentalTrack(std::string, std::unique_ptr<juce::AudioPluginInstance>);
        ~InstrumentalTrack() = default;

        InstrumentalTrack(InstrumentalTrack&&) = default;
        InstrumentalTrack& operator=(InstrumentalTrack&&) = default;

        InstrumentalTrack(const InstrumentalTrack&) = delete;
        InstrumentalTrack& operator=(const InstrumentalTrack&) = delete;

        const juce::String info();
        void add_midi_message(const juce::MidiMessage&, double);

        void render_midifile_into_wav(std::string, std::string);
    private:
        std::string m_name;
        std::unique_ptr<juce::AudioPluginInstance> m_audio_plugin_instance;
        juce::MidiMessageSequence m_midi_message_sequence;
};
