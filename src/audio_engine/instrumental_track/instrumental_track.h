#pragma once

#include "../../juce_header.h"
#include "../../../external/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
#include "../../../external/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
#include "../../../external/JUCE/modules/juce_audio_formats/juce_audio_formats.h"
#include <memory>
#include <string>

/**
* @brief Хранит vst плагин и взаимодействует с ним
*/
class InstrumentalTrack {
public:
    static std::unique_ptr<juce::AudioPluginInstance> load_plugin(juce::File, float = 44100.0, int = 512); 
    
    InstrumentalTrack(std::string, std::unique_ptr<juce::AudioPluginInstance>);
    ~InstrumentalTrack();
    
    InstrumentalTrack(InstrumentalTrack&&) = default;
    InstrumentalTrack& operator=(InstrumentalTrack&&) = default;
    
    InstrumentalTrack(const InstrumentalTrack&) = delete;
    InstrumentalTrack& operator=(const InstrumentalTrack&) = delete;
    
    const juce::String info();
    
    void prepareToPlay(double sampleRate, int blockSize);
    void releaseResources();
    
    void renderAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer);
    void addMidiMessage(const juce::MidiMessage&, double);
    
    void renderMidifileIntoWav(std::string, std::string);
    void renderSequenceIntoWav(juce::MidiMessageSequence sequence, std::string wavPath);
    
    juce::AudioPluginInstance* getPluginInstance() { return m_audio_plugin_instance.get(); }
    
private:
    std::string m_name;
    std::unique_ptr<juce::AudioPluginInstance> m_audio_plugin_instance;
    juce::MidiMessageSequence m_midi_message_sequence;
    double m_sampleRate = 44100.0;
    int m_blockSize = 512;
};
